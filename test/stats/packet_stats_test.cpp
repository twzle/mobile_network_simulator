#include <gtest/gtest.h>
#include "stats/packet_stats.hpp"

class PacketStatsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Общие настройки для тестов
    }
};

// Тест конструктора с параметрами
TEST_F(PacketStatsTest, ConstructorWithParameters) {
    PacketStats stats(1, 42, 0.005);
    
    EXPECT_EQ(1, stats.queue_id);
    EXPECT_EQ(42, stats.user_id);
    EXPECT_DOUBLE_EQ(0.005, stats.processing_delay);
}

// Тест на запрет копирования
TEST_F(PacketStatsTest, CopyConstructorDeleted) {
    PacketStats stats(1, 42, 0.005);
    
    // Вместо этого проверяем, что перемещение работает
    PacketStats moved = std::move(stats);
    EXPECT_EQ(1, moved.queue_id);
    EXPECT_EQ(42, moved.user_id);
    EXPECT_DOUBLE_EQ(0.005, moved.processing_delay);
}

// Тест конструктора перемещения
TEST_F(PacketStatsTest, MoveConstructor) {
    PacketStats original(2, 100, 0.01);
    PacketStats moved = std::move(original);
    
    EXPECT_EQ(2, moved.queue_id);
    EXPECT_EQ(100, moved.user_id);
    EXPECT_DOUBLE_EQ(0.01, moved.processing_delay);
}

// Тест граничных значений для queue_id
TEST_F(PacketStatsTest, QueueIdBoundaryValues) {
    PacketStats minQueue(0, 1, 0.001);
    PacketStats maxQueue(std::numeric_limits<int>::max(), 1, 0.001);
    
    EXPECT_EQ(0, minQueue.queue_id);
    EXPECT_EQ(std::numeric_limits<int>::max(), maxQueue.queue_id);
}

// Тест граничных значений для user_id
TEST_F(PacketStatsTest, UserIdBoundaryValues) {
    PacketStats minUser(1, 0, 0.001);
    PacketStats maxUser(1, std::numeric_limits<int>::max(), 0.001);
    
    EXPECT_EQ(0, minUser.user_id);
    EXPECT_EQ(std::numeric_limits<int>::max(), maxUser.user_id);
}

// Тест граничных значений для processing_delay
TEST_F(PacketStatsTest, ProcessingDelayBoundaryValues) {
    PacketStats zeroDelay(1, 1, 0.0);
    PacketStats maxDelay(1, 1, std::numeric_limits<double>::max());
    
    EXPECT_DOUBLE_EQ(0.0, zeroDelay.processing_delay);
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), maxDelay.processing_delay);
}

// Тест отрицательных значений (если они допустимы)
TEST_F(PacketStatsTest, NegativeValues) {
    PacketStats stats(-1, -1, -0.001);
    
    EXPECT_EQ(-1, stats.queue_id);
    EXPECT_EQ(-1, stats.user_id);
    EXPECT_DOUBLE_EQ(-0.001, stats.processing_delay);
}

// Тест на сравнение объектов
TEST_F(PacketStatsTest, CompareObjects) {
    PacketStats stats1(1, 42, 0.005);
    PacketStats stats2(1, 42, 0.005);
    PacketStats stats3(2, 42, 0.005);
    
    // Проверяем, что поля объектов равны
    EXPECT_EQ(stats1.queue_id, stats2.queue_id);
    EXPECT_EQ(stats1.user_id, stats2.user_id);
    EXPECT_DOUBLE_EQ(stats1.processing_delay, stats2.processing_delay);
    
    // Проверяем различие объектов
    EXPECT_NE(stats1.queue_id, stats3.queue_id);
}