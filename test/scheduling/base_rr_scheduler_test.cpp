#include <gtest/gtest.h>
#include "scheduling/RR/base_rr_scheduler.hpp"

class TestableBaseRRScheduler : public BaseRRScheduler
{
public:
    // Реализация чисто виртуальных методов (если они есть)
    void run() override
    {
        return;
    }
};

class BaseRRSchedulerTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        User::reset_last_id();

        testScheduler().set_tti_duration(0.001);
        testScheduler().set_resource_block_per_tti_limit(100);
        testScheduler().set_channel_sync_interval(0.010);
        testScheduler().set_base_cqi(10);
        testScheduler().set_users_per_tti_limit(4);
        testScheduler().set_tti_stats(100, 4, 2, 0.001, 1000);

        // Выполняется один раз перед всеми тестами
        BSConfig bs_config(0.0, 0.0, 30.0);
        testScheduler().set_base_station(bs_config);

        std::vector<UserConfig> user_configs{
            UserConfig(100.0, 50.0, 1.5, 1.0, "left", 1.0, 0.5, 100),
            UserConfig(-100.0, 50.0, 1.5, 2.0, "right", 1.0, 0.5, 100)};
        testScheduler().set_users(user_configs, 100);
    }

    static TestableBaseRRScheduler &testScheduler()
    {
        static TestableBaseRRScheduler instance;
        return instance;
    }

    TestableBaseRRScheduler &scheduler = testScheduler();
};

TEST_F(BaseRRSchedulerTest, InitialState) {
    EXPECT_EQ(0, scheduler.get_initial_queue());
    EXPECT_EQ(0, scheduler.get_stats().packet_count);
}

TEST_F(BaseRRSchedulerTest, SetInitialQueue) {
    scheduler.set_initial_queue(2);
    EXPECT_EQ(2, scheduler.get_initial_queue());
    
    scheduler.set_initial_queue(0);
    EXPECT_EQ(0, scheduler.get_initial_queue());
}

TEST_F(BaseRRSchedulerTest, GetRelativeQueueId) {
    // Добавляем 3 очереди
    for (int i = 0; i < 3; ++i) {
        scheduler.schedule(PacketQueue(1.0, 10));
    }
    
    scheduler.set_initial_queue(1);
    
    EXPECT_EQ(1, scheduler.get_relative_queue_id(0));
    EXPECT_EQ(2, scheduler.get_relative_queue_id(1));
    EXPECT_EQ(0, scheduler.get_relative_queue_id(2)); // Зацикливание
    EXPECT_EQ(1, scheduler.get_relative_queue_id(3)); // Зацикливание
}

TEST_F(BaseRRSchedulerTest, CheckQueueRemainingPackets) {
    PacketQueue queue(1.0, 10);
    User* user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);
    
    // Пакет доступен по времени
    Packet packet1(5, 0, user);
    // Пакет еще не доступен
    Packet packet2(10, 1, user);
    
    queue.push(packet1);
    queue.push(packet2);
    
    scheduler.check_queue_remaining_scheduled_packets(queue, 0.001);
    
    // Проверяем, что очередь не изменилась (пакеты восстановлены)
    EXPECT_EQ(2, queue.size());
}

TEST_F(BaseRRSchedulerTest, SyncUserChannels) {
    // Проверяем синхронизацию каналов пользователей
    scheduler.sync_user_channels();
    
    // Проверяем, что CQI пользователей установлен в базовое значение
    User* user1 = scheduler.get_user_ptr(0);
    User* user2 = scheduler.get_user_ptr(1);
    
    ASSERT_NE(nullptr, user1);
    ASSERT_NE(nullptr, user2);
    
    EXPECT_EQ(10, user1->get_cqi()); // base_cqi = 10
    EXPECT_EQ(10, user2->get_cqi());
}

TEST_F(BaseRRSchedulerTest, ResetServedUsers) {
    // Проверяем сброс списка обслуженных пользователей
    scheduler.reset_served_users();
    // Этот тест в основном проверяет, что метод выполняется без ошибок
    // Детальную проверку можно добавить, если есть методы для проверки состояния
}

TEST_F(BaseRRSchedulerTest, FairnessStatsEvaluation) {
    // Проверяем оценку статистики справедливости
    scheduler.evaluate_fairness_stats(SchedulerState::UNDEFINED, false);
    scheduler.evaluate_fairness_stats(SchedulerState::UNDEFINED, true);
    
    auto& stats = scheduler.get_stats();
    EXPECT_EQ(stats.scheduler_fairness_for_users.size(), 0);
    EXPECT_EQ(stats.scheduler_fairness_for_queues.size(), 0);
}

TEST_F(BaseRRSchedulerTest, ThroughputStatsEvaluation) {
    // Проверяем оценку статистики пропускной способности
    scheduler.evaluate_throughput_stats(false);
    scheduler.evaluate_throughput_stats(true);
    
    auto& stats = scheduler.get_stats();
    EXPECT_EQ(stats.scheduler_throughput.size(), 2);
}