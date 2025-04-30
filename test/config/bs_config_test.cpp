#include "config/bs_config.hpp"
#include <gtest/gtest.h>
#include <climits>

class BSConfigTest : public ::testing::Test
{
protected:
    // Тестовые данные
    const double TEST_X = 1.5;
    const double TEST_Y = 2.5;
    const double TEST_Z = 3.5;
    const double TEST_FREQ = 2100.0; // МГц
    const double TEST_BW = 20.0;     // МГц
    const int TEST_POWER = 40;       // dBm
};

// Тест конструктора по умолчанию
TEST_F(BSConfigTest, DefaultConstructor)
{
    BSConfig config;
    EXPECT_DOUBLE_EQ(config.get_x(), 0.0);
    EXPECT_DOUBLE_EQ(config.get_y(), 0.0);
    EXPECT_DOUBLE_EQ(config.get_z(), 0.0);
    EXPECT_DOUBLE_EQ(config.get_carrier_frequency(), 0.0);
    EXPECT_DOUBLE_EQ(config.get_bandwidth(), 0.0);
    EXPECT_EQ(config.get_transmission_power(), 0);
}

// Тест параметризованного конструктора
TEST_F(BSConfigTest, ParameterizedConstructor)
{
    BSConfig config(TEST_X, TEST_Y, TEST_Z, TEST_FREQ, TEST_BW, TEST_POWER);

    EXPECT_DOUBLE_EQ(config.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(config.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(config.get_z(), TEST_Z);
    EXPECT_DOUBLE_EQ(config.get_carrier_frequency(), TEST_FREQ);
    EXPECT_DOUBLE_EQ(config.get_bandwidth(), TEST_BW);
    EXPECT_EQ(config.get_transmission_power(), TEST_POWER);
}

// Тест конструктора копирования
TEST_F(BSConfigTest, CopyConstructor)
{
    BSConfig original(TEST_X, TEST_Y, TEST_Z, TEST_FREQ, TEST_BW, TEST_POWER);
    BSConfig copy(original);

    EXPECT_DOUBLE_EQ(copy.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(copy.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(copy.get_z(), TEST_Z);
    EXPECT_DOUBLE_EQ(copy.get_carrier_frequency(), TEST_FREQ);
    EXPECT_DOUBLE_EQ(copy.get_bandwidth(), TEST_BW);
    EXPECT_EQ(copy.get_transmission_power(), TEST_POWER);
}

// Тест оператора присваивания
TEST_F(BSConfigTest, AssignmentOperator)
{
    BSConfig original(TEST_X, TEST_Y, TEST_Z, TEST_FREQ, TEST_BW, TEST_POWER);
    BSConfig assigned;
    assigned = original;

    EXPECT_DOUBLE_EQ(assigned.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(assigned.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(assigned.get_z(), TEST_Z);
    EXPECT_DOUBLE_EQ(assigned.get_carrier_frequency(), TEST_FREQ);
    EXPECT_DOUBLE_EQ(assigned.get_bandwidth(), TEST_BW);
    EXPECT_EQ(assigned.get_transmission_power(), TEST_POWER);

    // Проверка самоприсваивания
    assigned = assigned;
    EXPECT_DOUBLE_EQ(assigned.get_x(), TEST_X);
}

// Тесты геттеров
TEST_F(BSConfigTest, Getters)
{
    BSConfig config(TEST_X, TEST_Y, TEST_Z, TEST_FREQ, TEST_BW, TEST_POWER);

    EXPECT_DOUBLE_EQ(config.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(config.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(config.get_z(), TEST_Z);
    EXPECT_DOUBLE_EQ(config.get_carrier_frequency(), TEST_FREQ);
    EXPECT_DOUBLE_EQ(config.get_bandwidth(), TEST_BW);
    EXPECT_EQ(config.get_transmission_power(), TEST_POWER);
}

// Тест на точность double
TEST_F(BSConfigTest, DoublePrecision)
{
    const double precise_x = 1.23456789;
    const double precise_y = 9.87654321;
    const double precise_z = 0.00000001;
    const double precise_freq = 1234.56789;
    const double precise_bw = 0.987654321;

    BSConfig config(precise_x, precise_y, precise_z, precise_freq, precise_bw, TEST_POWER);

    EXPECT_DOUBLE_EQ(config.get_x(), precise_x);
    EXPECT_DOUBLE_EQ(config.get_y(), precise_y);
    EXPECT_DOUBLE_EQ(config.get_z(), precise_z);
    EXPECT_DOUBLE_EQ(config.get_carrier_frequency(), precise_freq);
    EXPECT_DOUBLE_EQ(config.get_bandwidth(), precise_bw);
}