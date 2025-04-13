#include "core/position.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <cmath>

class PositionTest : public ::testing::Test {
protected:
    // Тестовые данные
    const double TEST_X = 1.5;
    const double TEST_Y = 2.5;
    const double TEST_Z = 3.5;
    const double DELTA = 1e-9; // Для сравнения double
};

// Тест конструктора по умолчанию
TEST_F(PositionTest, DefaultConstructor) {
    Position pos;
    EXPECT_DOUBLE_EQ(pos.get_x(), 0.0);
    EXPECT_DOUBLE_EQ(pos.get_y(), 0.0);
    EXPECT_DOUBLE_EQ(pos.get_z(), 0.0);
}

// Тест параметризованного конструктора
TEST_F(PositionTest, ParameterizedConstructor) {
    Position pos(TEST_X, TEST_Y, TEST_Z);
    EXPECT_DOUBLE_EQ(pos.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(pos.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(pos.get_z(), TEST_Z);
}

// Тест конструктора копирования
TEST_F(PositionTest, CopyConstructor) {
    Position original(TEST_X, TEST_Y, TEST_Z);
    Position copy(original);
    
    EXPECT_DOUBLE_EQ(copy.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(copy.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(copy.get_z(), TEST_Z);
}

// Тест оператора присваивания
TEST_F(PositionTest, AssignmentOperator) {
    Position original(TEST_X, TEST_Y, TEST_Z);
    Position assigned;
    assigned = original;
    
    EXPECT_DOUBLE_EQ(assigned.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(assigned.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(assigned.get_z(), TEST_Z);
    
    // Проверка самоприсваивания
    assigned = assigned;
    EXPECT_DOUBLE_EQ(assigned.get_x(), TEST_X);
}

// Тесты геттеров и сеттеров
TEST_F(PositionTest, SettersAndGetters) {
    Position pos;
    
    pos.set_x(TEST_X);
    pos.set_y(TEST_Y);
    pos.set_z(TEST_Z);
    
    EXPECT_DOUBLE_EQ(pos.get_x(), TEST_X);
    EXPECT_DOUBLE_EQ(pos.get_y(), TEST_Y);
    EXPECT_DOUBLE_EQ(pos.get_z(), TEST_Z);
}

// Тест 2D расстояния
TEST_F(PositionTest, Distance2D) {
    Position pos1(1.0, 2.0, 3.0);
    Position pos2(4.0, 6.0, 9.0);
    
    double expected = 5.0; // sqrt((4-1)^2 + (6-2)^2) = 5
    double actual = pos1.get_distance_2d(pos2);
    
    EXPECT_NEAR(actual, expected, DELTA);
}

// Тест 3D расстояния
TEST_F(PositionTest, Distance3D) {
    Position pos1(1.0, 2.0, 3.0);
    Position pos2(4.0, 6.0, 15.0);
    
    double expected = 13.0; // sqrt(5^2 + (15-3)^2) = 13
    double actual = pos1.get_distance_3d(pos2);
    
    EXPECT_NEAR(actual, expected, DELTA);
}

// Тест нулевого расстояния
TEST_F(PositionTest, ZeroDistance) {
    Position pos1(TEST_X, TEST_Y, TEST_Z);
    Position pos2(pos1);
    
    EXPECT_DOUBLE_EQ(pos1.get_distance_2d(pos2), 0.0);
    EXPECT_DOUBLE_EQ(pos1.get_distance_3d(pos2), 0.0);
}

// Тест оператора вывода
TEST_F(PositionTest, OutputOperator) {
    Position pos(TEST_X, TEST_Y, TEST_Z);
    std::ostringstream oss;
    
    oss << pos;
    
    std::string expected = "Position: (1.5, 2.5, 3.5)";
    EXPECT_EQ(oss.str(), expected);
}

// Тест с отрицательными координатами
TEST_F(PositionTest, NegativeCoordinates) {
    Position pos1(-1.0, -2.0, -3.0);
    Position pos2(-4.0, -6.0, -15.0);
    
    EXPECT_NEAR(pos1.get_distance_2d(pos2), 5.0, DELTA);
    EXPECT_NEAR(pos1.get_distance_3d(pos2), 13.0, DELTA);
}

// Тест с максимально точными значениями
TEST_F(PositionTest, HighPrecision) {
    const double precise_val = 1.234567890123456789;
    Position pos(precise_val, precise_val, precise_val);
    
    EXPECT_DOUBLE_EQ(pos.get_x(), precise_val);
    EXPECT_DOUBLE_EQ(pos.get_y(), precise_val);
    EXPECT_DOUBLE_EQ(pos.get_z(), precise_val);
}