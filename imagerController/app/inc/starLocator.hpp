#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <ctime>

class StarLocator{
public:
    enum Status{
        OK,
        N_OK
    };

    struct ImageInfo{
        cv::Mat m_buffer;
        int format;
        bool is_color;
    };

    struct StarInfo{
        double ra;
        double dec;
        int brightness;
    };

    struct StarLocalizationRegion{
        int x_start;
        int x_end;
        int y_start;
        int y_end;
    };

    struct StarCentroid{
        float x_cent;
        float y_cent;
        float brightness;
    };

    enum CentroidAlgorithm{
        COG,
        WCOG,
        IWCOG,
    };

    Status addToBuffer(uint8_t* buffer, uint32_t width, uint32_t heigth, int format, bool is_color);
    std::vector<StarInfo> findStars();
    cv::Mat filterAndTreshold(cv::Mat& image_to_filter, cv::Mat& im_filtered);
    std::vector<StarLocalizationRegion> findStarsRegions(cv::Mat& image_to_filter);
    std::vector<StarCentroid> findStarCentroids(const cv::Mat& im_base,const std::vector<StarLocalizationRegion> regions, CentroidAlgorithm alg);
protected:
    std::vector<StarCentroid> findStarCentroidsCOG(const cv::Mat& im_base,const std::vector<StarLocalizationRegion> regions);
    std::vector<StarCentroid> findStarCentroidsWCOG(const cv::Mat& im_base,const std::vector<StarLocalizationRegion> regions, std::vector<StarCentroid> initial_centroids = {});
    std::vector<StarCentroid> findStarCentroidsIWCOG(const cv::Mat& im_base,const std::vector<StarLocalizationRegion> regions);
private:
    float gaussianDistribution(float x, float y, float deviation);
    ImageInfo m_current_image;
    std::vector<ImageInfo> m_images_buffer;
    uint8_t m_image_buffer_max_size = 5;
};

