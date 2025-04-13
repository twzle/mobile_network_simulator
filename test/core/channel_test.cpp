#include "core/channel.hpp"
#include <gtest/gtest.h>
#include <cmath>

class ChannelTest : public ::testing::Test {
protected:
    const double FREQ_2GHZ = 2000.0;
    const double FREQ_28GHZ = 28000.0;
    const int POWER_40W = 40;
    const double BS_HEIGHT = 30.0;
    const double UE_HEIGHT = 1.5;
    const double DISTANCE_100M = 100.0;
    const double DELTA = 1e-6;
};

// Test default constructor
TEST_F(ChannelTest, DefaultConstructor) {
    Channel ch;
    EXPECT_DOUBLE_EQ(ch.get_carrier_frequency(), 0.0);
    EXPECT_EQ(ch.get_power_bs_transmitted(), 0);
    EXPECT_DOUBLE_EQ(ch.get_constant_offset(), 0.0);
}

// Test parameterized constructor with different area types
TEST_F(ChannelTest, ParameterizedConstructor) {
    // Dense Urban
    Channel ch1(FREQ_2GHZ, POWER_40W, "Dense Urban");
    EXPECT_DOUBLE_EQ(ch1.get_constant_offset(), 3.0);
    
    // Urban
    Channel ch2(FREQ_2GHZ, POWER_40W, "Urban");
    EXPECT_DOUBLE_EQ(ch2.get_constant_offset(), 0.0);
    
    // Suburban
    Channel ch3(FREQ_28GHZ, POWER_40W, "Suburban");
    double expected_offset = -(2 * pow(log10(FREQ_28GHZ/28), 2) + 5.4);
    EXPECT_NEAR(ch3.get_constant_offset(), expected_offset, DELTA);
    
    // Unknown area type (should default to Urban)
    Channel ch4(FREQ_2GHZ, POWER_40W, "Rural");
    EXPECT_DOUBLE_EQ(ch4.get_constant_offset(), 0.0);
}

// Test path loss calculation
TEST_F(ChannelTest, PathLossCalculation) {
    Channel ch(FREQ_2GHZ, POWER_40W, "Urban");
    
    // Calculate expected values manually
    double log_freq = log10(FREQ_2GHZ);
    double log_height_bs = log10(BS_HEIGHT);
    double log_distance = log10(DISTANCE_100M);
    double a_height_user = (1.1 * log_freq - 0.7) * UE_HEIGHT - (1.56 * log_freq - 0.8);
    
    double expected_pl = 46.3 + 33.9 * log_freq - 13.82 * log_height_bs -
                       a_height_user + 
                       (44.9 - 6.55 * log_height_bs) * log_distance;
    
    double actual_pl = ch.get_path_loss(DISTANCE_100M, BS_HEIGHT, UE_HEIGHT);
    
    EXPECT_NEAR(actual_pl, expected_pl, DELTA);
}

// Test received signal power calculation
TEST_F(ChannelTest, ReceivedSignalPower) {
    Channel ch(FREQ_2GHZ, POWER_40W, "Urban");
    double path_loss = ch.get_path_loss(DISTANCE_100M, BS_HEIGHT, UE_HEIGHT);
    double expected_power = POWER_40W - path_loss;
    
    EXPECT_NEAR(ch.get_recieved_signal_power(path_loss), expected_power, DELTA);
}

// Test SINR calculation
TEST_F(ChannelTest, SinrCalculation) {
    Channel ch(FREQ_2GHZ, POWER_40W, "Urban");
    double rx_power = 20.0; // dBm
    double noise_power = 5.0; // dBm
    double interference = 2.0; // dBm
    
    double expected_sinr = rx_power - noise_power - interference;
    EXPECT_DOUBLE_EQ(ch.get_sinr(rx_power, noise_power, interference), expected_sinr);
}

// Test noise power calculation
TEST_F(ChannelTest, NoisePowerCalculation) {
    Channel ch;
    double k_Boltzmann = 1.380649e-23;
    double T = 290.0;
    double B = 20e6;
    double expected_noise = 10 * log10(k_Boltzmann * T * B);
    
    EXPECT_NEAR(ch.get_noise_power(), expected_noise, DELTA);
}

// Test interference power (currently always zero)
TEST_F(ChannelTest, InterferencePower) {
    Channel ch;
    EXPECT_DOUBLE_EQ(ch.get_interference_power(), 0.0);
}

// Test edge cases
TEST_F(ChannelTest, EdgeCases) {
    // Very high frequency
    Channel ch1(100000.0, POWER_40W, "Suburban");
    EXPECT_TRUE(std::isfinite(ch1.get_path_loss(DISTANCE_100M, BS_HEIGHT, UE_HEIGHT)));
    
    // Very short distance (1m)
    Channel ch2(FREQ_2GHZ, POWER_40W, "Urban");
    EXPECT_TRUE(std::isfinite(ch2.get_path_loss(1.0, BS_HEIGHT, UE_HEIGHT)));
    
    // Very tall base station
    EXPECT_TRUE(std::isfinite(ch2.get_path_loss(DISTANCE_100M, 300.0, UE_HEIGHT)));
    
    // Very tall user equipment
    EXPECT_TRUE(std::isfinite(ch2.get_path_loss(DISTANCE_100M, BS_HEIGHT, 10.0)));
}