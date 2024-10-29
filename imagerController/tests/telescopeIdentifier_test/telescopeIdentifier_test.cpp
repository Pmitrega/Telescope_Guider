#include <iostream>
#include <gtest/gtest.h>
#include "telescopeIdentifier.hpp"
#include <fstream>

class TelescopeIdentifierTestFixture : public testing::Test{
    public:
};


TEST_F(TelescopeIdentifierTestFixture, identify_non_moving){
    TelescopeIdentifier tel_identifier(3.75, 240);
    std::vector<StarLocator::StarCentroid> locs = {
    {795.847,	65.9354, 0},
    {794.839,	68.9108, 0},
    {794.852,	71.9554, 0},
    {793.507,	75.2985, 0},
    {793.830, 	78.3532, 0},
    {791.969,	81.9446, 0},
    {791.471,	83.9827, 0},
    {791.376,	87.3227, 0},
    {789.965,	90.8668, 0},
    {788.429,	94.4528, 0},
    {789.371,	97.4413, 0},
    {788.387,	98.9679, 0},
    {787.902,	103.362, 0},
    {787.299,	105.913, 0},
    {786.908,	109.921, 0},
    {786.363,	112.459, 0},
    {785.409,	115.974, 0},
    {785.845,	118.474, 0},
    {784.873,	121.412, 0},
    {782.890, 	125.317, 0},
    {782.021,	128.736, 0},
    {782.243,	130.978, 0},
    {781.331,	134.887, 0},
    {780.424,	137.775, 0},
    {779.852,   140.759, 0},
    {778.974,   143.039, 0},
    {778.338,   146.821, 0},
    {777.375,   149.301, 0},
    {776.800,   152.798, 0},
    {775.882,   156.279, 0},
    {775.395,   158.386, 0}
    };
    for(auto it = locs.cbegin(); it < locs.cend(); it++){
        tel_identifier.estimateNonMoving(*it, 2000);
    }
    EXPECT_TRUE(false);
}
