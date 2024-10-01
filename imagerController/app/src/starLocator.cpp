#include "starLocator.hpp"
#include "logger.hpp"
#include <memory>
#include <cmath>
#include <numbers>
#include <cfloat>

#define DEBUG_IMAGES false
#if DEBUG_IMAGES == true
#define DEBUG_DISPLAY(name, image) { \
                                   cv::imshow(name, image); \
                                   cv::waitKey(0); \
                                   }
#else
#define   DEBUG_DISPLAY(name, image)  {}
#endif

StarLocator::Status StarLocator::addToBuffer(uint8_t* buffer, uint32_t width, uint32_t heigth,
                                             int format, bool is_color)
{
    if(m_images_buffer.size() <= m_image_buffer_max_size){
        cv::Mat image(heigth, width, format, reinterpret_cast<uint16_t*>(buffer));
        ImageInfo im_info{std::move(image), is_color};
        m_images_buffer.push_back(std::move(im_info));
    }
    else{
        LOG_WARNING("Image Buffer is full, can't add next image \r\n");
    }
    return OK;
}


std::vector<StarLocator::StarInfo> StarLocator::findStars(){
    if(m_images_buffer.size() == 0){
        return {};
    }
    m_current_image = m_images_buffer[0];
    m_images_buffer.erase(m_images_buffer.begin());
    cv::Mat im_filtered;
    cv::GaussianBlur(m_current_image.m_buffer, im_filtered, {5,5}, 0);
    double im_min, im_max;
    double im_mean = cv::mean(im_filtered).val[0];
    cv::minMaxLoc(im_filtered, &im_min, &im_max);
    cv::Mat im_thresholded;
    int threshold = (im_mean + (im_max - im_mean)/4);

    cv::threshold(im_filtered, im_thresholded, threshold ,UINT16_MAX - 1, cv::THRESH_BINARY);
    cv::imwrite("out.png", im_thresholded);
    cv::imwrite("out_filt.png", im_filtered);
    return {};
}


std::vector<StarLocator::StarLocalizationRegion> StarLocator::findStarsRegions(cv::Mat& I_base){
    cv::Mat im_thre;
    DEBUG_DISPLAY("Not filtered", I_base);
    filterAndTreshold(I_base, im_thre);
    DEBUG_DISPLAY("Filtered and thresholded", im_thre);
    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    im_thre.convertTo(im_thre, CV_8U);
    auto el_num = cv::connectedComponentsWithStats(im_thre, labels,stats, centroids, 8, CV_16U);
    std::vector<StarLocalizationRegion> regions;
    for(int i = 1; i < el_num; i++){
        StarLocalizationRegion reg;
        reg.x_start = stats.at<int>(i, cv::CC_STAT_LEFT);
        reg.y_start = stats.at<int>(i, cv::CC_STAT_TOP);
        reg.x_end = stats.at<int>(i, cv::CC_STAT_LEFT) + stats.at<int>(i, cv::CC_STAT_WIDTH);
        reg.y_end = stats.at<int>(i, cv::CC_STAT_TOP) + stats.at<int>(i, cv::CC_STAT_HEIGHT);
        regions.push_back(reg);
    }
    return regions;
}

std::vector<StarLocator::StarCentroid> StarLocator::findStarCentroids(const cv::Mat& im_base, const std::vector<StarLocalizationRegion> regions, CentroidAlgorithm alg){
    std::vector<StarLocator::StarCentroid> centroids;
    switch (alg)
    {
    case COG:
        centroids = findStarCentroidsCOG(im_base, regions);
        break;
    case WCOG:
        centroids = findStarCentroidsWCOG(im_base, regions);
        break;
    case IWCOG:
        centroids = findStarCentroidsIWCOG(im_base, regions);
        break;
    default:
        break;
    }
    return centroids;
}

std::vector<StarLocator::StarCentroid> StarLocator::findStarCentroidsCOG(const cv::Mat& im_base, const std::vector<StarLocalizationRegion> regions){
    std::vector<StarLocator::StarCentroid> centroids;
    const int COG_power = 4;
    for(auto reg = regions.cbegin(); reg < regions.cend(); reg ++){
        StarCentroid centroid;
        centroid.x_cent = 0;
        centroid.y_cent = 0;
        float total_brightness = 0;
        for(int w = reg->x_start; w <= reg->x_end; w++){
            for(int h = reg->y_start; h <= reg->y_end; h++){
                float brightness = static_cast<float>(pow(im_base.at<uint16_t>(h,w), COG_power));
                centroid.x_cent += w * brightness;
                centroid.y_cent += h * brightness;
                total_brightness += brightness;
            }
        }
        centroid.x_cent = centroid.x_cent / total_brightness;
        centroid.y_cent = centroid.y_cent / total_brightness;
        centroids.push_back(centroid);
    }
    return centroids;
}

float StarLocator::gaussianDistribution(float x, float y, float deviation){
    const float A = 1/(std::numbers::pi_v<float> * deviation * deviation);
    return A*expf((-x*x  -y*y)/(2*deviation*deviation));
}

std::vector<StarLocator::StarCentroid> StarLocator::findStarCentroidsWCOG(const cv::Mat& im_base,const std::vector<StarLocator::StarLocalizationRegion> regions, std::vector<StarLocator::StarCentroid> initial_centroids){
    if(initial_centroids.size() == 0){
        for(auto reg = regions.cbegin(); reg < regions.cend(); reg++){
            StarCentroid centr;
            centr.x_cent = (reg->x_start+ reg->x_end)/2.0f;
            centr.y_cent = (reg->y_start+reg->y_end)/2.0f;
            initial_centroids.push_back(centr);
        }
    }
    std::vector<StarLocator::StarCentroid> centroids;
    const float deviation  = 4;
    assert(regions.size() == initial_centroids.size());
    for(int i = 0; i < regions.size(); i++){
        StarCentroid centroid;
        centroid.x_cent = 0;
        centroid.y_cent = 0;
        float total_brightness = 0;
        for(int w = regions[i].x_start; w <= regions[i].x_end; w++){
            for(int h = regions[i].y_start; h <= regions[i].y_end; h++){
                auto x_diff = w - initial_centroids[i].x_cent;
                auto y_diff = h - initial_centroids[i].y_cent;
                float gauss_dist = gaussianDistribution(x_diff, y_diff, deviation);
                float brightness = static_cast<float>(im_base.at<uint16_t>(h,w)) * gauss_dist;
                centroid.x_cent += w * brightness;
                centroid.y_cent += h * brightness;
                total_brightness += static_cast<float>(im_base.at<uint16_t>(h,w)) * gauss_dist;
            }
        }
        centroid.x_cent = centroid.x_cent / total_brightness;
        centroid.y_cent = centroid.y_cent / total_brightness;
        centroid.brightness = total_brightness;
        centroids.push_back(centroid);
    }
    return centroids;

}

std::vector<StarLocator::StarCentroid> StarLocator::findStarCentroidsIWCOG(const cv::Mat& im_base,const std::vector<StarLocalizationRegion> regions){
    const int interation_number = 10;
    std::vector<StarCentroid> centroids;
    for(auto reg = regions.cbegin(); reg < regions.cend(); reg++){
            StarCentroid centr;
            centr.x_cent = (reg->x_start+ reg->x_end)/2.0f;
            centr.y_cent = (reg->y_start+reg->y_end)/2.0f;
            centroids.push_back(centr);
        }
    for(int i = 0; i < interation_number; i++){
        centroids = findStarCentroidsWCOG(im_base, regions, centroids);
    }
    return centroids;
}

cv::Mat StarLocator::filterAndTreshold(cv::Mat& image_to_filter, cv::Mat& im_filtered){
    cv::Mat helper_mat;
    cv::blur(image_to_filter, im_filtered, {5,5});
    GaussianBlur(im_filtered, im_filtered, {5,5}, 2, 2);
    DEBUG_DISPLAY("Filtered ", im_filtered);
    double min, max;
    cv::minMaxLoc(im_filtered, &min, &max);
    im_filtered.convertTo(im_filtered, CV_16U);
    cv::threshold(im_filtered, im_filtered, min*6/2, UINT16_MAX, cv::THRESH_BINARY);
    return im_filtered;
}