#include <gtest/gtest.h>
#include "stats/throughput_stats.hpp"
#include "core/user.hpp"
#include "core/position.hpp"
#include "core/mobility.hpp"

class ThroughputStatsTest : public ::testing::Test {
protected:
    void SetUp() override {
        User::reset_last_id();

        // Инициализация ThroughputStats
        stats.initialize(100, 3, 2, 0.001, 10);
        
        // Инициализация тестовых пользователей
        Position pos1(100.0, 50.0, 1.5);
        Position pos2(-100.0, 50.0, 1.5);
        Mobility mob1(1.0, "left");
        Mobility mob2(2.0, "right");
        
        user1 = std::make_unique<User>(10, pos1, mob1, 100, 1.0);
        user2 = std::make_unique<User>(10, pos2, mob2, 100, 1.0);
    }

    ThroughputStats stats;
    std::unique_ptr<User> user1;
    std::unique_ptr<User> user2;
};

TEST_F(ThroughputStatsTest, MarkUserAsResourceCandidate) {
    stats.mark_user_as_resource_candidate(user1.get());
    stats.mark_queue_as_resource_candidate(0);
    
    // Проверяем через calculate_throughput_for_scheduler
    stats.calculate_throughput_for_scheduler();
    EXPECT_TRUE(stats.is_valid_throughput_for_scheduler());
}

TEST_F(ThroughputStatsTest, AddAllocatedDataToUser) {
    stats.add_allocated_effective_data_to_user(user1.get(), 100);
    stats.add_allocated_effective_data_to_queue(0, 50);
    stats.add_allocated_effective_data_to_total(150, 15);
    
    stats.calculate_throughput_for_scheduler();
    EXPECT_GT(stats.get_throughput_for_scheduler(), 0);
    EXPECT_TRUE(stats.is_valid_throughput_for_scheduler());
}

TEST_F(ThroughputStatsTest, CalculateThroughputWithUsers) {
    // Добавляем пользователей как кандидатов
    stats.mark_user_as_resource_candidate(user1.get());
    stats.mark_user_as_resource_candidate(user2.get());
    stats.mark_queue_as_resource_candidate(0);
    
    // Добавляем данные
    stats.add_allocated_effective_data_to_user(user1.get(), 500000); // 500KB
    stats.add_allocated_effective_data_to_user(user2.get(), 500000); // 500KB
    stats.add_allocated_effective_data_to_total(1000000, 50); // 1MB
    
    stats.calculate_throughput_for_scheduler();
    
    // Ожидаем около 8 Мбит/мс (1MB * 8 / 1ms)
    EXPECT_NEAR(8.0, stats.get_throughput_for_scheduler(), 0.1);
    EXPECT_TRUE(stats.is_valid_throughput_for_scheduler());
}

TEST_F(ThroughputStatsTest, UserResourceAllocation) {
    // Проверяем распределение ресурсов между пользователями
    stats.add_allocated_effective_data_to_user(user1.get(), 300);
    stats.add_allocated_effective_data_to_user(user2.get(), 700);
    stats.add_allocated_effective_data_to_total(1000, 10);
    
    stats.calculate_throughput_for_scheduler();
    stats.calculate_unused_resources_for_scheduler();
    
    EXPECT_GT(stats.get_throughput_for_scheduler(), 0);
    EXPECT_LT(stats.get_unused_resources_for_scheduler(), 1.0);
    EXPECT_TRUE(stats.is_valid_throughput_for_scheduler());
    EXPECT_TRUE(stats.is_valid_unused_resources_for_scheduler());
}

TEST_F(ThroughputStatsTest, MultipleUsersSameQueue) {
    // Оба пользователя в одной очереди
    stats.mark_queue_as_resource_candidate(0);
    stats.add_allocated_effective_data_to_user(user1.get(), 400);
    stats.add_allocated_effective_data_to_user(user2.get(), 600);
    stats.add_allocated_effective_data_to_queue(0, 1000);
    stats.add_allocated_effective_data_to_total(1000, 20);
    
    stats.calculate_throughput_for_scheduler();
    
    EXPECT_NEAR(0.008, stats.get_throughput_for_scheduler(), 0.0001);
    EXPECT_TRUE(stats.is_valid_throughput_for_scheduler());
}

TEST_F(ThroughputStatsTest, UserWithoutAllocation) {
    // Пользователь отмечен как кандидат, но не получил ресурсов
    stats.mark_user_as_resource_candidate(user1.get());
    stats.mark_queue_as_resource_candidate(0);
    
    stats.calculate_throughput_for_scheduler();
    stats.calculate_unused_resources_for_scheduler();
    
    EXPECT_DOUBLE_EQ(0, stats.get_throughput_for_scheduler());
    EXPECT_DOUBLE_EQ(1.0, stats.get_unused_resources_for_scheduler());
    EXPECT_TRUE(stats.is_valid_throughput_for_scheduler());
    EXPECT_TRUE(stats.is_valid_unused_resources_for_scheduler());
}