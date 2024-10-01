#include <iostream>
#include <gtest/gtest.h>
#include "starLocator.hpp"
#include <fstream>

class StarLocatorTestFixture : public testing::Test{
    public:
    void readFile(std::string filename, char* buffer, long buff_len){
        std::ifstream fin;
        fin.open(filename.c_str(), std::ios::binary | std::ios::in);
        fin.read(buffer, buff_len);
        EXPECT_EQ(fin.is_open(), true) << "Can\'t open file " << filename;
        fin.close();
    }

    cv::Mat readFileWithExt(std::string filename){
        cv::Mat im = cv::imread(filename, 1);
        cv::extractChannel(im, im, 2);
        return im;
    }
    int findBrightestIndex(std::vector<StarLocator::StarCentroid> centroids){
        float brightest = 0.0f;
        int index = -1;
        for(int i =0; i < centroids.size(); i++){
            if(centroids[i].brightness > brightest){
                brightest = centroids[i].brightness;
                index = i;
            }
        }
        return index;
    }
};

/***********************Finding star regions tests*************************** */
/*Test 1*/
TEST_F(StarLocatorTestFixture, finding_regions_expected_number_of_stars_test1){
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    readFile("test_im1.raw", buffer, buffer_len);
    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    auto star_regions = star_locator.findStarsRegions(image);
    for(int i = 0; i < star_regions.size(); i++){
        std::cout << star_regions[i].x_start <<" "<< star_regions[i].x_end << " "
        << star_regions[i].y_start <<" "<< star_regions[i].y_end <<std::endl;
    }
    EXPECT_GE(star_regions.size(),3) << "Not enougth stars has been found";
    EXPECT_LE(star_regions.size(),5) << "Too much stars has been found";
}
/*Test 2*/
TEST_F(StarLocatorTestFixture, finding_regions_expected_number_of_stars_test2){
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    readFile("./series_2000ms/image00.raw", buffer, buffer_len);
    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    auto star_regions = star_locator.findStarsRegions(image);
    for(int i = 0; i < star_regions.size(); i++){
        std::cout << star_regions[i].x_start <<" "<< star_regions[i].x_end << " "
        << star_regions[i].y_start <<" "<< star_regions[i].y_end <<std::endl;
    }
    EXPECT_GE(star_regions.size(),12) << "Not enougth stars has been found";
    EXPECT_LE(star_regions.size(),18) << "Too much stars has been found";
}

/**********************************Star centroid finding********************************* */
TEST_F(StarLocatorTestFixture, star_region_centroids_COG_sanity){
    const std::vector<StarLocator::StarLocalizationRegion> regions ={
        {800 , 806 , 29,  34},
        {46  , 47  , 297, 298},
        {569 , 629 , 296, 359},
        {1254, 1269, 404, 419},
        {21  , 30  , 812, 819}
    };
    const std::vector<StarLocator::StarCentroid> expected_centroids = {
        {803.0 , 32.0},
        {46.5  , 297.5},
        {599.0 , 327.5},
        {1261.5, 411.5},
        {25.5  , 815.5}
    };
    const float epsilon = 2.f;
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    readFile("test_im1.raw", buffer, buffer_len);

    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    auto star_centroids = star_locator.findStarCentroids(image, regions, StarLocator::COG);
    for(int i =0; i < star_centroids.size(); i++){
        auto norm = pow(star_centroids[i].x_cent- expected_centroids[i].x_cent,2) + pow((star_centroids[i].y_cent- expected_centroids[i].y_cent),2);
        EXPECT_NEAR(norm, 0, epsilon*epsilon);
        EXPECT_NEAR(star_centroids[i].x_cent, expected_centroids[i].x_cent, epsilon);
        EXPECT_NEAR(star_centroids[i].y_cent, expected_centroids[i].y_cent, epsilon);
    }

}


TEST_F(StarLocatorTestFixture, star_region_centroids_WCOG_sanity){
    const std::vector<StarLocator::StarLocalizationRegion> regions ={
        {800 , 806 , 29,  34},
        {46  , 47  , 297, 298},
        {569 , 629 , 296, 359},
        {1254, 1269, 404, 419},
        {21  , 30  , 812, 819}
    };
    const std::vector<StarLocator::StarCentroid> expected_centroids = {
        {803.0 , 32.0},
        {46.5  , 297.5},
        {599.0 , 327.5},
        {1261.5, 411.5},
        {25.5  , 815.5}
    };
    const float epsilon = 2.f;
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    readFile("test_im1.raw", buffer, buffer_len);

    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    auto star_centroids = star_locator.findStarCentroids(image, regions, StarLocator::WCOG);
    for(int i =0; i < star_centroids.size(); i++){
        auto norm = pow(star_centroids[i].x_cent- expected_centroids[i].x_cent,2) + pow((star_centroids[i].y_cent- expected_centroids[i].y_cent),2);
        EXPECT_NEAR(norm, 0, epsilon*epsilon);
        EXPECT_NEAR(star_centroids[i].x_cent, expected_centroids[i].x_cent, epsilon);
        EXPECT_NEAR(star_centroids[i].y_cent, expected_centroids[i].y_cent, epsilon);
    }

}

TEST_F(StarLocatorTestFixture, star_region_centroids_IWCOG_sanity){
    const std::vector<StarLocator::StarLocalizationRegion> regions ={
        {800 , 806 , 29,  34},
        {46  , 47  , 297, 298},
        {569 , 629 , 296, 359},
        {1254, 1269, 404, 419},
        {21  , 30  , 812, 819}
    };
    const std::vector<StarLocator::StarCentroid> expected_centroids = {
        {803.0 , 32.0},
        {46.5  , 297.5},
        {599.0 , 327.5},
        {1261.5, 411.5},
        {25.5  , 815.5}
    };
    const float epsilon = 2.f;
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    readFile("test_im1.raw", buffer, buffer_len);

    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    auto star_centroids = star_locator.findStarCentroids(image, regions, StarLocator::IWCOG);
    for(int i =0; i < star_centroids.size(); i++){
        auto norm = pow(star_centroids[i].x_cent- expected_centroids[i].x_cent,2) + pow((star_centroids[i].y_cent- expected_centroids[i].y_cent),2);
        EXPECT_NEAR(norm, 0, epsilon*epsilon);
        EXPECT_NEAR(star_centroids[i].x_cent, expected_centroids[i].x_cent, epsilon);
        EXPECT_NEAR(star_centroids[i].y_cent, expected_centroids[i].y_cent, epsilon);
    }

}

TEST_F(StarLocatorTestFixture, star_region_centroids_IWCOG_linearity){
    const float epsilon = 2.f;
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    std::string filename;
    for(int i =0; i <=30; i++){
        if(i < 10){
                filename = std::string("image0") + std::to_string(i) + ".raw";
        }
        else{
                filename = std::string("image") + std::to_string(i) + ".raw";
        }
        readFile("./series_2000ms/" + filename, buffer, buffer_len);
        cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
        auto regions = star_locator.findStarsRegions(image);
        auto star_centroids = star_locator.findStarCentroids(image, regions, StarLocator::IWCOG);
        int idx = findBrightestIndex(star_centroids);
        std::cout << star_centroids[idx].x_cent << ", " << star_centroids[idx].y_cent << std::endl;
    }

    EXPECT_TRUE(true);
}
