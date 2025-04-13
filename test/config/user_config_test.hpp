#ifndef USER_CONFIG_TEST_HPP
#define USER_CONFIG_TEST_HPP

#include "config/user_config.hpp"
#include <gtest/gtest.h>
#include <string>
#include <climits>

class UserConfigTest : public ::testing::Test {
protected:
    const double TEST_X = 1.1;
    const double TEST_Y = 2.2;
    const double TEST_Z = 3.3;
    const double TEST_SPEED = 10.5;
    const std::string TEST_DIRECTION = "north";
    const double TEST_QUANT = 0.5;
    const int TEST_PACKET_SIZE = 1500;
    const double TEST_TRAFFIC_PART = 0.75;
};

#endif // USER_CONFIG_TEST_HPP