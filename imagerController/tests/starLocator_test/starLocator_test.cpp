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
};


TEST_F(StarLocatorTestFixture, finding_regions_expected_number_of_stars_test){
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    readFile("test_im1.raw", buffer, buffer_len);
    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    auto star_regions = star_locator.findStarsRegions(image);
    EXPECT_GE(star_regions.size(),3) << "Not enougth stars has been found";
    std::cout << star_regions.size() << std::endl;
    EXPECT_LE(star_regions.size(),6) << "Too much stars has been found";
}


TEST_F(StarLocatorTestFixture, region_estimated_brightness){
    const int width = 1280;
    const int heigth = 960;
    const int bytes_per_pixel = 2;
    const int buffer_len = width*heigth*bytes_per_pixel;
    char buffer[buffer_len];
    StarLocator star_locator;
    readFile("test_im1.raw", buffer, buffer_len);
    cv::Mat image(heigth, width, CV_16U, reinterpret_cast<uint16_t*>(buffer));
    auto star_regions = star_locator.findStarsRegions(image);
}