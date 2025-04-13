#include "config/bs_config.hpp"
#include <gtest/gtest.h>

class BSConfigTest : public ::testing::Test {
protected:
    // Тестовые данные
    const double TEST_X = 1.5;
    const double TEST_Y = 2.5;
    const double TEST_Z = 3.5;
};

// Тест конструктора по умолчанию
TEST_F(BSConfigTest, DefaultConstructor) {
    BSConfig config;
    EXPECT_DOUBLE_EQ(config.get_x(), 0.0);
    EXPECT_DOUBLE_EQ(config.get_y(), 0.0);
    EXPECT_DOUBLE_EQ(config.get_z(), 0.0);
}

// Тест параметризованного конструктора
TEST_F(BSConfigTest, ParameterizedConstructor) {
    BSConfig config(TEST_X, TEST_Y, TEST_Z);
    EXPECT_DOUBLE_EQ(config.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(config.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(config.get_z(), TEST_Z);
}

// Тест конструктора копирования
TEST_F(BSConfigTest, CopyConstructor) {
    BSConfig original(TEST_X, TEST_Y, TEST_Z);
    BSConfig copy(original);
    
    EXPECT_DOUBLE_EQ(copy.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(copy.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(copy.get_z(), TEST_Z);
}

// Тест оператора присваивания
TEST_F(BSConfigTest, AssignmentOperator) {
    BSConfig original(TEST_X, TEST_Y, TEST_Z);
    BSConfig assigned;
    assigned = original;
    
    EXPECT_DOUBLE_EQ(assigned.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(assigned.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(assigned.get_z(), TEST_Z);
    
    // Проверка самоприсваивания
    assigned = assigned;
    EXPECT_DOUBLE_EQ(assigned.get_x(), TEST_X);
}

// Тесты геттеров
TEST_F(BSConfigTest, Getters) {
    BSConfig config(TEST_X, TEST_Y, TEST_Z);
    
    EXPECT_DOUBLE_EQ(config.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(config.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(config.get_z(), TEST_Z);
}

// Тест на точность double
TEST_F(BSConfigTest, DoublePrecision) {
    const double precise_x = 1.23456789;
    const double precise_y = 9.87654321;
    const double precise_z = 0.00000001;
    
    BSConfig config(precise_x, precise_y, precise_z);
    
    EXPECT_DOUBLE_EQ(config.get_x(), precise_x);
    EXPECT_DOUBLE_EQ(config.get_y(), precise_y);
    EXPECT_DOUBLE_EQ(config.get_z(), precise_z);
}