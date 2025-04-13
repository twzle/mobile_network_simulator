#ifndef BS_CONFIG_TEST_HPP
#define BS_CONFIG_TEST_HPP

#include "config/bs_config.hpp"
#include <gtest/gtest.h>

class BSConfigTest : public ::testing::Test {
protected:
    const double TEST_X = 1.5;
    const double TEST_Y = 2.5;
    const double TEST_Z = 3.5;
};

#endif // BS_CONFIG_TEST_HPP