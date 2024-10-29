
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "starLocator.hpp"

class TelescopeIdentifier{
public:
    TelescopeIdentifier(double pixel_size_um, double focal_length_mm) : m_pixel_size(pixel_size_um/10e6), m_focal_length_m(focal_length_mm/1000){};
    StarLocator::StarCentroid findGuidingStar(std::vector<StarLocator::StarCentroid> stars);
    void estimateNonMoving(const StarLocator::StarCentroid centroid,double sampling_ms,uint16_t max_centroids_numb = 30);
    void estimateMovingRa(double speed_dps);
    void estimateMovingDec(double speed_dps);
    void clearNonMoving(){m_non_moving_list = {};};
    void clearRaMoving(){m_ra_moving_list = {};};
    void clearDecMoving(){m_dec_moving_list = {};};
private:
    double m_pixel_size;
    double m_estimated_dec;
    double m_relative_ra;
    double m_focal_length_m;
    std::pair<double,double> m_sky_dec_vector;
    std::pair<double,double> m_sky_ra_vector;
    std::pair<double,double> m_telescope_dec_vector;
    std::pair<double,double> m_telscope_ra_vector;
    std::pair<double,double> m_telescope_main_axis_loc;
    std::vector<StarLocator::StarCentroid> m_non_moving_list = {};
    std::vector<StarLocator::StarCentroid> m_ra_moving_list = {};
    std::vector<StarLocator::StarCentroid> m_dec_moving_list {};
    std::vector<StarLocator::StarCentroid> m_last_stars = {};
};