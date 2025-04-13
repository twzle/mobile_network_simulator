#include "config/user_config.hpp"
#include <gtest/gtest.h>
#include <string>
#include <climits>

class UserConfigTest : public ::testing::Test {
protected:
    // Тестовые данные
    const double TEST_X = 1.1;
    const double TEST_Y = 2.2;
    const double TEST_Z = 3.3;
    const double TEST_SPEED = 10.5;
    const std::string TEST_DIRECTION = "north";
    const double TEST_QUANT = 0.5;
    const int TEST_PACKET_SIZE = 1500;
    const double TEST_TRAFFIC_PART = 0.75;
};

// Тест конструктора и геттеров
TEST_F(UserConfigTest, ConstructorAndGetters) {
    UserConfig config(
        TEST_X, TEST_Y, TEST_Z,
        TEST_SPEED, TEST_DIRECTION,
        TEST_QUANT, TEST_PACKET_SIZE, TEST_TRAFFIC_PART
    );

    // Проверка координат
    EXPECT_DOUBLE_EQ(config.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(config.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(config.get_z(), TEST_Z);

    // Проверка параметров движения
    EXPECT_DOUBLE_EQ(config.get_speed(), TEST_SPEED);
    EXPECT_EQ(config.get_direction(), TEST_DIRECTION);

    // Проверка сетевых параметров
    EXPECT_DOUBLE_EQ(config.get_quant(), TEST_QUANT);
    EXPECT_EQ(config.get_packet_size(), TEST_PACKET_SIZE);
    EXPECT_DOUBLE_EQ(config.get_traffic_part(), TEST_TRAFFIC_PART);
}

// Тест с граничными значениями
TEST_F(UserConfigTest, BoundaryValues) {
    const double min_val = 0.0;
    const double max_val = 1000.0;
    const std::string empty_str = "";

    UserConfig config(
        min_val, min_val, min_val,
        max_val, empty_str,
        min_val, 1, max_val
    );

    EXPECT_DOUBLE_EQ(config.get_x(), min_val);
    EXPECT_DOUBLE_EQ(config.get_speed(), max_val);
    EXPECT_TRUE(config.get_direction().empty());
    EXPECT_EQ(config.get_packet_size(), 1);
}

// Тест с отрицательными значениями (где допустимо)
TEST_F(UserConfigTest, NegativeValues) {
    UserConfig config(
        -1.5, -2.5, -3.5,
        -10.0, "south",
        -0.1, -1500, -0.5
    );

    EXPECT_DOUBLE_EQ(config.get_x(), -1.5);
    EXPECT_DOUBLE_EQ(config.get_speed(), -10.0);
    EXPECT_EQ(config.get_packet_size(), -1500);
}

// Тест с максимально точными double значениями
TEST_F(UserConfigTest, DoublePrecision) {
    const double precise_val = 1.234567890123456;

    UserConfig config(
        precise_val, precise_val, precise_val,
        precise_val, "precision",
        precise_val, 1234, precise_val
    );

    EXPECT_DOUBLE_EQ(config.get_x(), precise_val);
    EXPECT_DOUBLE_EQ(config.get_quant(), precise_val);
}

// Тест с разными направлениями
TEST_F(UserConfigTest, DifferentDirections) {
    std::string directions[] = {"north", "south", "east", "west", ""};
    
    for (const auto& dir : directions) {
        UserConfig config(
            0, 0, 0,
            0, dir,
            0, 0, 0
        );
        EXPECT_EQ(config.get_direction(), dir);
    }
}

// Тест с экстремальными значениями packet_size
TEST_F(UserConfigTest, PacketSizeExtremes) {
    UserConfig min_packet(
        0, 0, 0,
        0, "min",
        0, INT_MIN, 0
    );
    
    UserConfig max_packet(
        0, 0, 0,
        0, "max",
        0, INT_MAX, 0
    );

    EXPECT_EQ(min_packet.get_packet_size(), INT_MIN);
    EXPECT_EQ(max_packet.get_packet_size(), INT_MAX);
}