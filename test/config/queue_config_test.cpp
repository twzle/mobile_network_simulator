#include "config/queue_config.hpp"
#include <gtest/gtest.h>
#include <limits>

class QueueConfigTest : public ::testing::Test {
protected:
    // Стандартные тестовые значения
    const int TEST_PACKET_COUNT = 100;
    const double TEST_QUANT = 0.5;
    const double TEST_LAMBDA = 1.0;  // пакетов в секунду
};

// Тест конструктора и геттеров с нормальными значениями
TEST_F(QueueConfigTest, ConstructorAndGettersNormalValues) {
    QueueConfig config(TEST_PACKET_COUNT, TEST_QUANT, TEST_LAMBDA);
    
    EXPECT_EQ(config.get_packet_count(), TEST_PACKET_COUNT);
    EXPECT_DOUBLE_EQ(config.get_quant(), TEST_QUANT);
    EXPECT_DOUBLE_EQ(config.get_time_lambda(), TEST_LAMBDA);
}

// Тест граничных значений для packet_count
TEST_F(QueueConfigTest, BoundaryPacketCount) {
    // Минимальное значение
    QueueConfig minConfig(0, TEST_QUANT, TEST_LAMBDA);
    EXPECT_EQ(minConfig.get_packet_count(), 0);
    
    // Максимальное значение
    QueueConfig maxConfig(std::numeric_limits<int>::max(), TEST_QUANT, TEST_LAMBDA);
    EXPECT_EQ(maxConfig.get_packet_count(), std::numeric_limits<int>::max());
}

// Тест граничных значений для quant
TEST_F(QueueConfigTest, BoundaryQuant) {
    // Минимальное положительное значение
    QueueConfig minConfig(TEST_PACKET_COUNT, std::numeric_limits<double>::min(), TEST_LAMBDA);
    EXPECT_DOUBLE_EQ(minConfig.get_quant(), std::numeric_limits<double>::min());
    
    // Максимальное значение
    QueueConfig maxConfig(TEST_PACKET_COUNT, std::numeric_limits<double>::max(), TEST_LAMBDA);
    EXPECT_DOUBLE_EQ(maxConfig.get_quant(), std::numeric_limits<double>::max());
    
    // Нулевое значение
    QueueConfig zeroConfig(TEST_PACKET_COUNT, 0.0, TEST_LAMBDA);
    EXPECT_DOUBLE_EQ(zeroConfig.get_quant(), 0.0);
}

// Тест граничных значений для time_lambda
TEST_F(QueueConfigTest, BoundaryTimeLambda) {
    // Минимальное положительное значение
    QueueConfig minConfig(TEST_PACKET_COUNT, TEST_QUANT, std::numeric_limits<double>::min());
    EXPECT_DOUBLE_EQ(minConfig.get_time_lambda(), std::numeric_limits<double>::min());
    
    // Максимальное значение
    QueueConfig maxConfig(TEST_PACKET_COUNT, TEST_QUANT, std::numeric_limits<double>::max());
    EXPECT_DOUBLE_EQ(maxConfig.get_time_lambda(), std::numeric_limits<double>::max());
    
    // Нулевое значение
    QueueConfig zeroConfig(TEST_PACKET_COUNT, TEST_QUANT, 0.0);
    EXPECT_DOUBLE_EQ(zeroConfig.get_time_lambda(), 0.0);
}

// Тест отрицательных значений
TEST_F(QueueConfigTest, NegativeValues) {
    // Отрицательное количество пакетов
    QueueConfig negPackets(-TEST_PACKET_COUNT, TEST_QUANT, TEST_LAMBDA);
    EXPECT_EQ(negPackets.get_packet_count(), -TEST_PACKET_COUNT);
    
    // Отрицательный квант времени
    QueueConfig negQuant(TEST_PACKET_COUNT, -TEST_QUANT, TEST_LAMBDA);
    EXPECT_DOUBLE_EQ(negQuant.get_quant(), -TEST_QUANT);
    
    // Отрицательная интенсивность
    QueueConfig negLambda(TEST_PACKET_COUNT, TEST_QUANT, -TEST_LAMBDA);
    EXPECT_DOUBLE_EQ(negLambda.get_time_lambda(), -TEST_LAMBDA);
}

// Тест точности double для quant и time_lambda
TEST_F(QueueConfigTest, DoublePrecision) {
    const double precise_quant = 0.123456789012345;
    const double precise_lambda = 0.987654321098765;
    
    QueueConfig config(TEST_PACKET_COUNT, precise_quant, precise_lambda);
    
    EXPECT_DOUBLE_EQ(config.get_quant(), precise_quant);
    EXPECT_DOUBLE_EQ(config.get_time_lambda(), precise_lambda);
}

// Тест на одинаковые значения
TEST_F(QueueConfigTest, SameValues) {
    QueueConfig config1(10, 0.1, 1.0);
    QueueConfig config2(10, 0.1, 1.0);
    
    EXPECT_EQ(config1.get_packet_count(), config2.get_packet_count());
    EXPECT_DOUBLE_EQ(config1.get_quant(), config2.get_quant());
    EXPECT_DOUBLE_EQ(config1.get_time_lambda(), config2.get_time_lambda());
}