#include "config/settings.hpp"
#include "config/user_config.hpp"
#include "config/bs_config.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <climits>


class SettingsTest : public ::testing::Test
{
protected:
    static bool initialized;
    static void lazy_initialize()
    {
        if (!initialized)
        {
            StandardManager::initialize();
            initialized = true;
        }
    }

    void SetUp() override
    {
        lazy_initialize();

        // Подготовка тестовых данных
        test_users = {
            UserConfig(1.0, 2.0, 3.0, 10.0, "north", 0.5, 1500, 0.75),
            UserConfig(4.0, 5.0, 6.0, 20.0, "south", 0.7, 1000, 0.85)};
        test_bs_config = BSConfig(10.0, 20.0, 30.0);
    }

    std::vector<UserConfig> test_users;
    BSConfig test_bs_config;
};

bool SettingsTest::initialized = false;


// Тест конструктора и базовых геттеров
TEST_F(SettingsTest, ConstructorAndBasicGetters)
{
    Settings settings(
        5,                    // launches
        "5G",                 // standard_type
        10,                   // base_cqi
        "1ms",                // tti_duration
        "100ms",              // channel_sync_interval
        "DefaultRRScheduler", // scheduler_type
        20.0,                 // bandwidth
        1000,                 // packet_count
        1500,                 // packet_size
        4,                    // queue_count
        0.5,                  // queue_quant
        100,                  // queue_limit
        0.1,                  // time_lambda
        test_users,           // user_configs
        test_bs_config,       // bs_config
        2100.0,               // carrier_frequency
        40,                   // bs_transmission_power
        "urban",              // area_type
        10,                   // users_per_tti_limit
        100,                  // throughput_history_size
        50                    // fairness_history_size
    );

    // Проверка простых параметров
    EXPECT_EQ(settings.get_launches(), 5);
    EXPECT_EQ(settings.get_standard_type(), "5G");
    EXPECT_EQ(settings.get_base_cqi(), 10);
    EXPECT_EQ(settings.get_scheduler_name(), "DefaultRRScheduler");
    EXPECT_DOUBLE_EQ(settings.get_bandwidth(), 20.0);
    EXPECT_EQ(settings.get_packet_count(), 1000);
    EXPECT_EQ(settings.get_packet_size(), 1500);
    EXPECT_EQ(settings.get_queue_count(), 4);
    EXPECT_DOUBLE_EQ(settings.get_queue_quant(), 0.5);
    EXPECT_EQ(settings.get_queue_limit(), 100);
    EXPECT_DOUBLE_EQ(settings.get_time_lambda(), 0.1);
    EXPECT_EQ(settings.get_user_count(), 2);
    EXPECT_DOUBLE_EQ(settings.get_carrier_frequency(), 2100.0);
    EXPECT_EQ(settings.get_bs_transmission_power(), 40);
    EXPECT_EQ(settings.get_area_type(), "urban");
    EXPECT_EQ(settings.get_users_per_tti_limit(), 10);
    EXPECT_EQ(settings.get_throughput_history_size(), 100);
    EXPECT_EQ(settings.get_fairness_history_size(), 50);

    // Проверка сложных объектов
    auto bs_config = settings.get_bs_config();
    EXPECT_DOUBLE_EQ(bs_config.get_x(), 10.0);

    auto user_configs = settings.get_user_configs();
    EXPECT_EQ(user_configs.size(), 2);
    EXPECT_DOUBLE_EQ(user_configs[0].get_x(), 1.0);
}

// Тест получения экземпляра планировщика
TEST_F(SettingsTest, SchedulerInstanceCreation)
{
    // Проверка всех типов планировщиков
    std::vector<std::string> scheduler_types = {
        "DefaultRRScheduler",
        "FixedDRRScheduler",
        "FixedDRRSchedulerWithUserQuant",
        "CyclicDRRScheduler",
        "CyclicDRRSchedulerWithUserQuant",
        "DefaultDRRScheduler",
        "DefaultDRRSchedulerWithUserQuant",
        "DefaultPFScheduler"};

    for (const auto &type : scheduler_types)
    {
        Settings settings(
            1, "5G", 10, "1ms", "100ms", type,
            20.0, 1000, 1500, 4, 0.5, 100, 0.1,
            test_users, test_bs_config,
            2100.0, 40, "urban", 10, 100, 50);

        auto scheduler = settings.get_scheduler_instance();
        EXPECT_NE(scheduler, nullptr) << "Failed for scheduler type: " << type;
    }

    // Проверка неверного типа планировщика
    Settings invalid_settings(
        1, "5G", 10, "1ms", "100ms", "InvalidScheduler",
        20.0, 1000, 1500, 4, 0.5, 100, 0.1,
        test_users, test_bs_config,
        2100.0, 40, "urban", 10, 100, 50);

    EXPECT_EQ(invalid_settings.get_scheduler_instance(), nullptr);
}

// Тест работы с пользовательскими конфигурациями
TEST_F(SettingsTest, UserConfigsHandling)
{
    Settings settings(
        1, "5G", 10, "1ms", "100ms", "DefaultRRScheduler",
        20.0, 1000, 1500, 4, 0.5, 100, 0.1,
        test_users, test_bs_config,
        2100.0, 40, "urban", 10, 100, 50);

    // Проверка количества пользователей
    EXPECT_EQ(settings.get_user_count(), 2);

    // Проверка получения конфигурации пользователя
    auto user_config = settings.get_user_config(0);
    EXPECT_DOUBLE_EQ(user_config.get_x(), 1.0);
    EXPECT_DOUBLE_EQ(user_config.get_speed(), 10.0);

    // Проверка получения всех конфигураций
    auto all_configs = settings.get_user_configs();
    EXPECT_EQ(all_configs.size(), 2);
}

// Тест вычисляемых параметров
TEST_F(SettingsTest, CalculatedParameters)
{
    Settings settings(
        1, "LTE", 10, "1ms", "10ms", "DefaultRRScheduler",
        20.0, 1000, 1500, 4, 0.5, 100, 0.1,
        test_users, test_bs_config,
        2100.0, 40, "urban", 10, 100, 50);

    // Проверка вычисления TTI (зависит от реализации StandardManager)
    EXPECT_EQ(settings.get_tti_value(), 0.001);

    // Проверка вычисления интервала синхронизации канала
    EXPECT_EQ(settings.get_channel_sync_interval_value(), 0.010);

    // Проверка вычисления количества RB
    EXPECT_EQ(settings.get_resource_block_per_tti_limit(), 100);

    // Проверка вычисления максимального размера пакета
    EXPECT_EQ(settings.get_packet_size_limit(), 3822);
}

// Тест граничных значений
TEST_F(SettingsTest, BoundaryValues)
{
    // Проверка минимальных значений
    Settings min_settings(
        0, "", 0, "", "", "",
        0.0, 0, 0, 0, 0.0, 0, 0.0,
        {}, BSConfig(0, 0, 0),
        0.0, 0, "", 0, 0, 0);

    EXPECT_EQ(min_settings.get_launches(), 0);
    EXPECT_EQ(min_settings.get_base_cqi(), 0);
    EXPECT_EQ(min_settings.get_user_count(), 0);

    // Проверка максимальных значений
    Settings max_settings(
        INT_MAX, "LTE", 255, "10ms", "1s", "DefaultPFScheduler",
        DBL_MAX, INT_MAX, INT_MAX, INT_MAX, DBL_MAX, INT_MAX, DBL_MAX,
        {UserConfig(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, "", DBL_MAX, INT_MAX, DBL_MAX)},
        BSConfig(DBL_MAX, DBL_MAX, DBL_MAX),
        DBL_MAX, INT_MAX, "rural", INT_MAX, INT_MAX, INT_MAX);

    EXPECT_EQ(max_settings.get_launches(), INT_MAX);
    EXPECT_DOUBLE_EQ(max_settings.get_bandwidth(), DBL_MAX);
}