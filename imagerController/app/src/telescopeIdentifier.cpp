#include "telescopeIdentifier.hpp"
#include <numbers>

void TelescopeIdentifier::estimateNonMoving(const StarLocator::StarCentroid centroid,double sampling_ms,uint16_t max_centroids_numb){
    m_non_moving_list.push_back(centroid);
    double x_speed = 0;
    double y_speed = 0;
    if(m_non_moving_list.size() < 2){
        return;
    }
    /*
        Now just average :/
    */
    x_speed = ((m_non_moving_list.cend()-1)->x_cent - m_non_moving_list.cbegin()->x_cent) /(m_non_moving_list.size()-1) / (sampling_ms/1000.0);
    y_speed = ((m_non_moving_list.cend()-1)->y_cent - m_non_moving_list.cbegin()->y_cent) /(m_non_moving_list.size()-1) / (sampling_ms/1000.0);
    std::cout << "pixel speed:"<< x_speed<< " "<<y_speed << std::endl;
    std::cout << "sec per pixel" << m_pixel_size/m_focal_length_m / std::numbers::pi * 180 * 60 * 60<< std::endl;
    x_speed =x_speed*m_pixel_size/m_focal_length_m / std::numbers::pi * 180 * 60 * 60 * 4; // ang sec per sec
    y_speed =y_speed*m_pixel_size/m_focal_length_m / std::numbers::pi * 180 * 60 * 60 * 4;
    std::cout << "speed sec per sec:"<< x_speed<< " "<<y_speed << std::endl;
    double v_lenght = std::sqrt(x_speed*x_speed + y_speed*y_speed);
    m_sky_ra_vector  = {x_speed/v_lenght, y_speed/v_lenght};
    if(m_sky_ra_vector.first * m_sky_dec_vector.second >0){
        m_sky_dec_vector = {m_sky_ra_vector.second, -m_sky_ra_vector.first};
    }
    else{
        m_sky_dec_vector = {-m_sky_ra_vector.second, m_sky_ra_vector.first};
    }
    std::cout << v_lenght << std::endl;
    if(v_lenght > 15){v_lenght = 15;};
    std::cout <<"speed:[asec/sec]"<< v_lenght<< std::endl;
    std::cout <<"Dec"<< std::acos(v_lenght/15.0)/std::numbers::pi * 180 << std::endl;
}