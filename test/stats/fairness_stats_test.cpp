#include <gtest/gtest.h>
#include "stats/fairness_stats.hpp"
#include "core/user.hpp"
#include "core/position.hpp"
#include "core/mobility.hpp"

class FairnessStatsTest : public ::testing::Test {
protected:
    void SetUp() override {
        User::reset_last_id();
        // Инициализация FairnessStats
        stats.initialize(3, 2, 0.001, 10);
        
        // Инициализация тестовых пользователей
        Position pos1(100.0, 50.0, 1.5);
        Position pos2(-100.0, 50.0, 1.5);
        Mobility mob1(1.0, "left");
        Mobility mob2(2.0, "right");
        
        user1 = std::make_unique<User>(10, pos1, mob1, 100, 1.0);
        user2 = std::make_unique<User>(10, pos2, mob2, 100, 1.0);
    }

    FairnessStats stats;
    std::unique_ptr<User> user1;
    std::unique_ptr<User> user2;
};

TEST_F(FairnessStatsTest, Initialization) {
    EXPECT_EQ(0, stats.get_current_history_size());
    EXPECT_EQ(10, stats.get_history_size_limit());
    EXPECT_EQ(3, stats.get_queue_count());
    EXPECT_EQ(2, stats.get_user_count());
    EXPECT_DOUBLE_EQ(0.001, stats.get_tti_duration());
}

TEST_F(FairnessStatsTest, Reset) {
    // Добавляем данные
    stats.add_allocated_effective_data_to_user(user1.get(), 100);
    stats.add_allocated_effective_data_to_queue(0, 100);
    stats.add_allocated_effective_data_to_total(100);
    stats.increment_current_history_size();
    
    stats.reset();
    
    EXPECT_EQ(0, stats.get_current_history_size());
    EXPECT_EQ(0, stats.get_total_allocated_effective_data_size());
}

TEST_F(FairnessStatsTest, HistorySizeOperations) {
    EXPECT_EQ(0, stats.get_current_history_size());
    
    stats.increment_current_history_size();
    EXPECT_EQ(1, stats.get_current_history_size());
    
    for (int i = 0; i < 9; ++i) {
        stats.increment_current_history_size();
    }
    
    EXPECT_TRUE(stats.is_history_size_limit_reached());
}

TEST_F(FairnessStatsTest, MarkResourceCandidates) {
    stats.mark_user_as_resource_candidate(user1.get());
    stats.mark_queue_as_resource_candidate(0);

    stats.add_allocated_effective_data_to_user(user1.get(), 100);
    stats.add_allocated_effective_data_to_queue(0, 100);
    stats.add_allocated_effective_data_to_total(100);

    stats.calculate_fairness_for_queues();
    stats.calculate_fairness_for_users();
    
    EXPECT_TRUE(stats.is_valid_fairness_for_queues() || 
               stats.is_valid_fairness_for_users());
}

TEST_F(FairnessStatsTest, AddAllocatedData) {
    stats.add_allocated_effective_data_to_user(user1.get(), 100);
    stats.add_allocated_effective_data_to_queue(0, 50);
    stats.add_allocated_effective_data_to_total(150);
    
    stats.calculate_fairness_for_queues();
    stats.calculate_fairness_for_users();
    
    EXPECT_TRUE(stats.is_valid_fairness_for_queues());
    EXPECT_TRUE(stats.is_valid_fairness_for_users());
}

TEST_F(FairnessStatsTest, PerfectFairnessForQueues) {
    // Идеальная справедливость - все очереди получили одинаковое количество данных
    stats.add_allocated_effective_data_to_queue(0, 100);
    stats.add_allocated_effective_data_to_queue(1, 100);
    stats.add_allocated_effective_data_to_total(200);
    
    stats.mark_queue_as_resource_candidate(0);
    stats.mark_queue_as_resource_candidate(1);
    
    stats.calculate_fairness_for_queues();
    
    EXPECT_DOUBLE_EQ(1.0, stats.get_fairness_for_queues());
    EXPECT_TRUE(stats.is_valid_fairness_for_queues());
}

TEST_F(FairnessStatsTest, PerfectFairnessForUsers) {
    // Идеальная справедливость - все пользователи получили одинаковое количество данных
    stats.add_allocated_effective_data_to_user(user1.get(), 100);
    stats.add_allocated_effective_data_to_user(user2.get(), 100);
    stats.add_allocated_effective_data_to_total(200);
    
    stats.mark_user_as_resource_candidate(user1.get());
    stats.mark_user_as_resource_candidate(user2.get());
    
    stats.calculate_fairness_for_users();
    
    EXPECT_DOUBLE_EQ(1.0, stats.get_fairness_for_users());
    EXPECT_TRUE(stats.is_valid_fairness_for_users());
}

TEST_F(FairnessStatsTest, UnfairDistribution) {
    // Несправедливое распределение - один пользователь получил все данные
    stats.add_allocated_effective_data_to_user(user1.get(), 200);
    stats.add_allocated_effective_data_to_user(user2.get(), 0);
    stats.add_allocated_effective_data_to_total(200);
    
    stats.mark_user_as_resource_candidate(user1.get());
    stats.mark_user_as_resource_candidate(user2.get());
    
    stats.calculate_fairness_for_users();
    
    EXPECT_LT(stats.get_fairness_for_users(), 1.0);
    EXPECT_GT(stats.get_fairness_for_users(), 0.0);
    EXPECT_TRUE(stats.is_valid_fairness_for_users());
}

TEST_F(FairnessStatsTest, NoDataButCandidates) {
    // Нет распределенных данных, но есть кандидаты
    stats.mark_user_as_resource_candidate(user1.get());
    stats.mark_queue_as_resource_candidate(0);
    
    stats.calculate_fairness_for_queues();
    stats.calculate_fairness_for_users();
    
    EXPECT_FALSE(stats.is_valid_fairness_for_queues());
    EXPECT_FALSE(stats.is_valid_fairness_for_users());
}

TEST_F(FairnessStatsTest, EdgeCases) {
    // Проверка с нулевыми лимитами
    FairnessStats zeroStats;
    zeroStats.initialize(0, 0, 0.0, 0);
    
    zeroStats.calculate_fairness_for_queues();
    zeroStats.calculate_fairness_for_users();
    
    EXPECT_FALSE(zeroStats.is_valid_fairness_for_queues());
    EXPECT_FALSE(zeroStats.is_valid_fairness_for_users());
}

TEST_F(FairnessStatsTest, InvalidUserOrQueue) {
    // Передача nullptr пользователя
    stats.mark_user_as_resource_candidate(nullptr);
    stats.add_allocated_effective_data_to_user(nullptr, 100);
    
    // Выход за границы очередей
    EXPECT_NO_THROW(stats.mark_queue_as_resource_candidate(10));
    EXPECT_NO_THROW(stats.add_allocated_effective_data_to_queue(10, 100));
    
    // Проверяем, что система не падает
    stats.calculate_fairness_for_queues();
    stats.calculate_fairness_for_users();
}