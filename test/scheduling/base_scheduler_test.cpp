#include <gtest/gtest.h>
#include "scheduling/base_scheduler.hpp"

class TestableBaseScheduler : public BaseScheduler
{
public:
    // Реализация чисто виртуальных методов (если они есть)
    void run() override
    {
        return;
    }

    // Можно добавить вспомогательные методы для тестирования
    void schedule(PacketQueue &&packet_queue) override
    {
        packet_queue.get_limit();
        return;
    }

    void evaluate_stats() override {};
    void evaluate_fairness_stats(
        SchedulerState scheduler_state, bool force_update) override
    {
        scheduler_state = SchedulerState::UNDEFINED;
        force_update = false;
        int i = 0;

        if (force_update)
        {
            scheduler_state = SchedulerState::IDLE;

            if (scheduler_state == SchedulerState::IDLE)
            {
                i++;
            }
        }
    };
    void evaluate_throughput_stats(bool force_update) override
    {
        force_update = false;
        int i = 0;

        if (force_update)
        {
            i++;
        }
    };

protected:
    void sync_user_channels() override
    {
        return;
    }
};

class BaseSchedulerTest : public ::testing::Test
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

    static TestableBaseScheduler &testScheduler()
    {
        static TestableBaseScheduler instance;
        return instance;
    }

    TestableBaseScheduler &scheduler = testScheduler();
};

TEST_F(BaseSchedulerTest, Initialization)
{
    // Проверка инициализации базовой станции
    EXPECT_NE(nullptr, scheduler.get_user_ptr(0));
    EXPECT_NE(nullptr, scheduler.get_user_ptr(1));

    EXPECT_EQ(nullptr, scheduler.get_user_ptr(2)); // Несуществующий пользователь
}

TEST_F(BaseSchedulerTest, UserManagement)
{
    // Проверка получения указателей на пользователей
    User *user1 = scheduler.get_user_ptr(0);
    User *user2 = scheduler.get_user_ptr(1);

    ASSERT_NE(nullptr, user1);
    ASSERT_NE(nullptr, user2);

    // Проверка позиций пользователей
    Position pos1 = user1->get_position();
    EXPECT_DOUBLE_EQ(100.0, pos1.get_x());
    EXPECT_DOUBLE_EQ(50.0, pos1.get_y());
    EXPECT_DOUBLE_EQ(1.5, pos1.get_z());

    Position pos2 = user2->get_position();
    EXPECT_DOUBLE_EQ(-100.0, pos2.get_x());
    EXPECT_DOUBLE_EQ(50.0, pos2.get_y());
    EXPECT_DOUBLE_EQ(1.5, pos2.get_z());
}

TEST_F(BaseSchedulerTest, PacketSizeConversion)
{
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    // Проверка конвертации размера пакета в RB
    int rb_count = scheduler.convert_packet_size_to_rb_number(user, 1000);
    EXPECT_GT(rb_count, 0);
    EXPECT_LT(rb_count, 100);

    // Проверка для другого размера
    int rb_count2 = scheduler.convert_packet_size_to_rb_number(user, 2000);
    EXPECT_GT(rb_count2, rb_count);
}

TEST_F(BaseSchedulerTest, StatsRecording) {
    User* user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);

    // Создаем тестовый пакет
    Packet packet(1, 0, user);

    // Записываем статистику
    double current_time = 0.001;
    int packet_size_bytes = 1500;
    int packet_size_rb = scheduler.convert_packet_size_to_rb_number(user, packet_size_bytes);

    scheduler.save_processed_packet_stats(packet, packet_size_bytes, packet_size_rb, current_time);

    // Проверяем статистику
    IterationStats& stats = scheduler.get_stats();

    EXPECT_EQ(stats.packet_stats.size(), 1);
}

TEST_F(BaseSchedulerTest, EdgeCases)
{
    // Проверка граничных случаев
    EXPECT_EQ(nullptr, scheduler.get_user_ptr(-1)); // Неверный ID

    // Конвертация нулевого размера пакета
    User *user = scheduler.get_user_ptr(0);
    ASSERT_NE(nullptr, user);
    int rb_count = scheduler.convert_packet_size_to_rb_number(user, 0);
    EXPECT_EQ(-1, rb_count);

    // Конвертация отрицательного размера
    rb_count = scheduler.convert_packet_size_to_rb_number(user, -100);
    EXPECT_EQ(-1, rb_count);

    // Конвертация слишком большого размера
    rb_count = scheduler.convert_packet_size_to_rb_number(user, 50000);
    EXPECT_EQ(-1, rb_count);
}