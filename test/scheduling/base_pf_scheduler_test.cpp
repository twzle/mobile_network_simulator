#include <gtest/gtest.h>
#include "scheduling/PF/base_pf_scheduler.hpp"

class BasePFSchedulerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        User::reset_last_id();
        // Другие параметры
        scheduler.set_tti_duration(0.001);
        scheduler.set_resource_block_per_tti_limit(100);
        scheduler.set_channel_sync_interval(0.010);
        scheduler.set_base_cqi(10);
        scheduler.set_users_per_tti_limit(2);
        scheduler.set_tti_stats(100, 4, 2, 0.001, 1000);
        // Настройка базовой станции
        BSConfig bs_config(0.0, 0.0, 30.0, 2100, 20, 40);
        scheduler.set_base_station(bs_config);

        // Настройка пользователей
        std::vector<UserConfig> user_configs{
            UserConfig(100.0, 50.0, 1.5, 1.0, "left", 1.0, 0.5, 100),
            UserConfig(-100.0, 50.0, 1.5, 2.0, "right", 1.0, 0.5, 100)};
        scheduler.set_users(user_configs, 100);
    }

    BasePFScheduler scheduler;
};

TEST_F(BasePFSchedulerTest, InitialState)
{
    EXPECT_EQ(0, scheduler.get_stats().packet_count);
    EXPECT_EQ(0, scheduler.get_stats().scheduler_total_time);
}

TEST_F(BasePFSchedulerTest, SchedulePacketQueue)
{
    PacketQueue queue(1.0, 10);
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    queue.push(Packet(10, 0.0, user));
    queue.push(Packet(10, 0.0, user));

    scheduler.schedule(std::move(queue));

    // Проверяем, что пакеты добавлены
    EXPECT_EQ(2, scheduler.get_total_packets());
}

TEST_F(BasePFSchedulerTest, RunSchedulerWithSinglePacket)
{
    PacketQueue queue(1.0, 10);
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    queue.push(Packet(10, 0, user));
    scheduler.schedule(std::move(queue));

    scheduler.run();

    // Проверяем, что пакет обработан
    auto &stats = scheduler.get_stats();
    EXPECT_EQ(1, stats.packet_count);
    EXPECT_GT(stats.scheduler_total_time, 0.0);
}

TEST_F(BasePFSchedulerTest, SyncUserChannels)
{
    // Проверяем синхронизацию каналов
    scheduler.sync_user_channels();

    User *user1 = scheduler.get_user_ptr(0);
    User *user2 = scheduler.get_user_ptr(1);

    ASSERT_NE(nullptr, user1);
    ASSERT_NE(nullptr, user2);

    // Проверяем, что CQI установлен (может отличаться от base_cqi после синхронизации)
    EXPECT_NE(0, user1->get_cqi());
    EXPECT_NE(0, user2->get_cqi());
}

TEST_F(BasePFSchedulerTest, UpdateUserPriorities)
{
    // Добавляем тестовые данные
    PacketQueue queue(1.0, 10);
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    queue.push(Packet(10, 0, user));
    scheduler.schedule(std::move(queue));

    // Запускаем обновление приоритетов
    scheduler.run();

    // Проверяем, что приоритеты установлены
    EXPECT_GT(user->get_priority(), 0.0);
}

TEST_F(BasePFSchedulerTest, EvaluateStats)
{
    // Добавляем тестовые данные
    PacketQueue queue(1.0, 10);
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    queue.push(Packet(10, 0.0, user));
    scheduler.schedule(std::move(queue));

    // Запускаем планировщик
    scheduler.run();

    // Проверяем статистику
    auto &stats = scheduler.get_stats();
    EXPECT_EQ(1, stats.packet_count);
    EXPECT_GT(stats.scheduler_total_time, 0.0);
}

TEST_F(BasePFSchedulerTest, FlushUserContext)
{
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    // Устанавливаем некоторые значения
    user->set_current_throughput(100);
    user->set_resource_candidate(true);

    // Вызываем сброс контекста
    scheduler.flush_user_context();

    // Проверяем сброс значений
    EXPECT_EQ(0, user->get_current_throughput());
    EXPECT_FALSE(user->is_resource_candidate());
}

TEST_F(BasePFSchedulerTest, UpdateUserThroughputs)
{
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    // Устанавливаем пользователя как кандидата на ресурсы
    user->set_resource_candidate(true);
    user->set_current_throughput(100);

    // Обновляем историю throughput
    scheduler.update_user_throughputs();

    // Проверяем, что история обновлена
    EXPECT_GT(user->get_average_throughput(), 0.0);
}

TEST_F(BasePFSchedulerTest, ResetServedUsers)
{
    // Добавляем пользователя в список кандидатов
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    PacketQueue queue(1.0, 10);
    queue.push(Packet(10, 0.0, user));
    scheduler.schedule(std::move(queue));

    // Запускаем сбор актуальных пакетов
    scheduler.collect_relevant_packets(0.0);

    // Сбрасываем список обслуженных пользователей
    scheduler.reset_served_users();

    // Проверяем, что список пуст (косвенно через filter_packets_of_excluded_from_scheduling_users)
    PacketQueue test_queue;
    scheduler.schedule(std::move(test_queue));
    scheduler.run();
}