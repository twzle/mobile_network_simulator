#include <gtest/gtest.h>
#include "stats/iteration_stats.hpp"

class IterationStatsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализация перед каждым тестом
        stats.packet_count = 0;
        stats.scheduler_total_time = 0.0;
        stats.scheduler_processing_time = 0.0;
        stats.scheduler_idle_time = 0.0;
        stats.scheduler_wait_time = 0.0;
    }

    IterationStats stats;
};

TEST_F(IterationStatsTest, QueueTimeOperations) {
    // Проверка работы с временем очередей
    stats.set_queue_total_time(0, 1.5);
    stats.set_queue_processing_time(0, 0.5);
    stats.set_queue_idle_time(0, 0.7);
    stats.set_queue_wait_time(0, 0.3);

    EXPECT_DOUBLE_EQ(1.5, stats.get_queue_total_time(0));
    EXPECT_DOUBLE_EQ(0.5, stats.get_queue_processing_time(0));
    EXPECT_DOUBLE_EQ(0.7, stats.get_queue_idle_time(0));
    EXPECT_DOUBLE_EQ(0.3, stats.get_queue_wait_time(0));

    // Инкремент времени
    stats.increment_queue_processing_time(0, 0.1);
    stats.increment_queue_idle_time(0, 0.1);
    stats.increment_queue_wait_time(0, 0.1);

    EXPECT_DOUBLE_EQ(0.6, stats.get_queue_processing_time(0));
    EXPECT_DOUBLE_EQ(0.8, stats.get_queue_idle_time(0));
    EXPECT_DOUBLE_EQ(0.4, stats.get_queue_wait_time(0));
}

TEST_F(IterationStatsTest, SchedulerTimeOperations) {
    // Проверка работы с временем планировщика
    stats.increment_scheduler_processing_time(0.5);
    stats.increment_scheduler_idle_time(0.3);
    stats.increment_scheduler_wait_time(0.2);

    EXPECT_DOUBLE_EQ(0.5, stats.scheduler_processing_time);
    EXPECT_DOUBLE_EQ(0.3, stats.scheduler_idle_time);
    EXPECT_DOUBLE_EQ(0.2, stats.scheduler_wait_time);
}

TEST_F(IterationStatsTest, UpdateQueueTimeStats) {
    // Проверка обновления статистики очереди по состоянию
    stats.update_queue_time_stats(PacketQueueState::PROCESSING, 0, 0.1);
    EXPECT_DOUBLE_EQ(0.1, stats.get_queue_processing_time(0));

    stats.update_queue_time_stats(PacketQueueState::IDLE, 0, 0.2);
    EXPECT_DOUBLE_EQ(0.2, stats.get_queue_idle_time(0));

    stats.update_queue_time_stats(PacketQueueState::WAIT, 0, 0.3);
    EXPECT_DOUBLE_EQ(0.3, stats.get_queue_wait_time(0));
}

TEST_F(IterationStatsTest, UpdateSchedulerTimeStats) {
    // Проверка обновления статистики планировщика по состоянию
    stats.update_scheduler_time_stats(SchedulerState::PROCESSING, 0.1);
    EXPECT_DOUBLE_EQ(0.1, stats.scheduler_processing_time);

    stats.update_scheduler_time_stats(SchedulerState::IDLE, 0.2);
    EXPECT_DOUBLE_EQ(0.2, stats.scheduler_idle_time);

    stats.update_scheduler_time_stats(SchedulerState::WAIT, 0.3);
    EXPECT_DOUBLE_EQ(0.3, stats.scheduler_wait_time);
}

TEST_F(IterationStatsTest, AddAndEvaluatePacketStats) {
    // Добавление статистики пакетов
    stats.add_queue_packet_stats(0, 1, 0.01);
    stats.add_queue_packet_stats(0, 1, 0.02);
    stats.add_queue_packet_stats(1, 2, 0.03);
    
    stats.packet_count = 3;
    
    // Вычисление статистик
    stats.evaluate(2, 2); // 2 очереди, 2 пользователя
    
    // Проверка средних задержек
    EXPECT_DOUBLE_EQ(0.015, stats.queue_average_packet_processing_delay[0]); // (0.01 + 0.02) / 2
    EXPECT_DOUBLE_EQ(0.03, stats.queue_average_packet_processing_delay[1]);
    EXPECT_DOUBLE_EQ(0.015, stats.user_average_packet_processing_delay[1]); // (0.01 + 0.02) / 2
    EXPECT_DOUBLE_EQ(0.03, stats.user_average_packet_processing_delay[2]);
}

TEST_F(IterationStatsTest, UpdateAndEvaluateFairnessStats) {
    // Обновление статистики справедливости
    stats.update_scheduler_fairness_for_queues(2, 0.8, true);
    stats.update_scheduler_fairness_for_queues(3, 0.9, true);
    stats.update_scheduler_fairness_for_users(1, 0.7, true);
    stats.update_scheduler_fairness_for_users(2, 0.8, true);
    
    // Вычисление средних
    stats.evaluate_fairness_for_queues_stats();
    stats.evaluate_fairness_for_users_stats();
    
    // Проверка расчетов
    EXPECT_NEAR(0.86, stats.scheduler_average_fairness_for_queues, 0.01); // (2*0.8 + 3*0.9)/5
    EXPECT_NEAR(0.766, stats.scheduler_average_fairness_for_users, 0.01); // (1*0.7 + 2*0.8)/3
}

TEST_F(IterationStatsTest, UpdateAndEvaluateThroughputStats) {
    // Обновление статистики пропускной способности
    stats.update_scheduler_throughput(10.0, true);
    stats.update_scheduler_throughput(20.0, true);
    stats.update_scheduler_throughput(30.0, true);
    
    // Обновление статистики неиспользованных ресурсов
    stats.update_scheduler_unused_resources(0.1, true);
    stats.update_scheduler_unused_resources(0.2, true);
    
    // Вычисление средних
    stats.evaluate_throughput_stats();
    stats.evaluate_unused_resources_stats();
    
    // Проверка расчетов
    EXPECT_DOUBLE_EQ(20.0, stats.scheduler_average_throughput); // (10+20+30)/3
    EXPECT_DOUBLE_EQ(0.15, stats.scheduler_average_unused_resources); // (0.1+0.2)/2
}

TEST_F(IterationStatsTest, EdgeCases) {
    // Проверка граничных случаев
    stats.update_scheduler_fairness_for_queues(0, 0.0, false);
    stats.update_scheduler_fairness_for_users(0, 0.0, false);
    stats.update_scheduler_throughput(0.0, false);
    stats.update_scheduler_unused_resources(0.0, false);
    
    // Не должно добавить невалидные данные
    stats.evaluate(0, 0);
    EXPECT_TRUE(stats.scheduler_fairness_for_queues.empty());
    EXPECT_TRUE(stats.scheduler_fairness_for_users.empty());
    EXPECT_TRUE(stats.scheduler_throughput.empty());
    EXPECT_TRUE(stats.scheduler_unused_resources.empty());
}

TEST_F(IterationStatsTest, ReleaseMemory) {
    // Проверка освобождения памяти
    stats.update_scheduler_throughput(10.0, true);
    stats.update_scheduler_unused_resources(0.1, true);
    stats.update_scheduler_fairness_for_queues(1, 0.8, true);
    stats.update_scheduler_fairness_for_users(1, 0.7, true);
    stats.add_queue_packet_stats(0, 1, 0.01);
    
    stats.release_memory_resources();
    
    // Проверка, что контейнеры пусты
    EXPECT_TRUE(stats.scheduler_throughput.empty());
    EXPECT_TRUE(stats.scheduler_unused_resources.empty());
    EXPECT_TRUE(stats.scheduler_fairness_for_queues.empty());
    EXPECT_TRUE(stats.scheduler_fairness_for_users.empty());
    EXPECT_TRUE(stats.packet_stats.empty());
}