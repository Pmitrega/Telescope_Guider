#include "starLocator.hpp"
#include "logger.hpp"
#include <memory>

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
    std::cout <<"thr" << threshold << std::endl;
    cv::threshold(im_filtered, im_thresholded, threshold ,UINT16_MAX - 1, cv::THRESH_BINARY);
    cv::imwrite("out.png", im_thresholded);
    cv::imwrite("out_filt.png", im_filtered);
    return {};
}


std::vector<StarLocator::StarLocalizationRegion> StarLocator::findStarsRegions(cv::Mat& I_base){
    cv::Mat im_filtered;
    filterAndTreshold(I_base, im_filtered);
    StarLocalizationRegion reg;
    reg.x_start = 0;
    reg.x_end = 10;
    reg.y_start = 0;
    reg.y_end = 10;
    return {reg};
}
cv::Mat StarLocator::filterAndTreshold(cv::Mat& image_to_filter, cv::Mat& im_filtered){
    GaussianBlur(image_to_filter, im_filtered, {3,3}, 1, 1);
    return im_filtered;
}