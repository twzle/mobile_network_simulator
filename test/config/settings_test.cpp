#include "config/settings.hpp"
#include "config/user_config.hpp"
#include "config/bs_config.hpp"
#include "config/queue_config.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <climits>
#include <vector>

class SettingsTest : public ::testing::Test {
protected:
    static bool initialized;
    static void lazy_initialize() {
        if (!initialized) {
            StandardManager::initialize();
            initialized = true;
        }
    }

    void SetUp() override {
        lazy_initialize();

        // Подготовка тестовых данных
        test_queue_configs = {
            QueueConfig(100, 0.5, 0.1),
            QueueConfig(200, 1.0, 0.1)
        };
        
        test_user_configs = {
            UserConfig(1.0, 2.0, 3.0, 10.0, "north", 0.5, 1500, 0.75),
            UserConfig(4.0, 5.0, 6.0, 20.0, "south", 0.7, 1000, 0.85)
        };
        
        test_bs_config = BSConfig(10.0, 20.0, 30.0, 2100.0, 20.0, 40);
    }

    std::vector<QueueConfig> test_queue_configs;
    std::vector<UserConfig> test_user_configs;
    BSConfig test_bs_config;
};

bool SettingsTest::initialized = false;

TEST_F(SettingsTest, ConstructorAndBasicGetters) {
    Settings settings(
        5,                      // launches
        "5G",                   // standard_type
        10,                     // base_cqi
        "1ms",                  // tti_duration
        "100ms",                // channel_sync_interval
        "DefaultRRScheduler",   // scheduler_type
        test_queue_configs,     // queue_configs
        test_user_configs,      // user_configs
        test_bs_config,         // bs_config
        "urban",               // area_type
        10,                    // users_per_tti_limit
        100,                  // throughput_history_size
        50                    // fairness_history_size
    );

    // Проверка простых параметров
    EXPECT_EQ(settings.get_launches(), 5);
    EXPECT_EQ(settings.get_standard_type(), "5G");
    EXPECT_EQ(settings.get_base_cqi(), 10);
    EXPECT_EQ(settings.get_scheduler_name(), "DefaultRRScheduler");
    EXPECT_DOUBLE_EQ(settings.get_bandwidth(), 20.0);
    EXPECT_EQ(settings.get_queue_count(), 2);
    EXPECT_EQ(settings.get_user_count(), 2);
    EXPECT_DOUBLE_EQ(settings.get_carrier_frequency(), 2100.0);
    EXPECT_EQ(settings.get_bs_transmission_power(), 40);
    EXPECT_EQ(settings.get_area_type(), "urban");
    EXPECT_EQ(settings.get_users_per_tti_limit(), 10);
    EXPECT_EQ(settings.get_throughput_history_size(), 100);
    EXPECT_EQ(settings.get_fairness_history_size(), 50);
    EXPECT_EQ(settings.get_total_packet_count(), 300); // 100 + 200

    // Проверка сложных объектов
    auto bs_config = settings.get_bs_config();
    EXPECT_DOUBLE_EQ(bs_config.get_x(), 10.0);

    auto queues = settings.get_queue_configs();
    EXPECT_EQ(queues.size(), 2);
    EXPECT_EQ(queues[0].get_packet_count(), 100);

    auto users = settings.get_user_configs();
    EXPECT_EQ(users.size(), 2);
    EXPECT_DOUBLE_EQ(users[0].get_x(), 1.0);
}

TEST_F(SettingsTest, SchedulerInstanceCreation) {
    std::vector<std::string> scheduler_types = {
        "DefaultRRScheduler",
        "FixedDRRScheduler",
        "FixedDRRSchedulerWithUserQuant",
        "CyclicDRRScheduler",
        "CyclicDRRSchedulerWithUserQuant",
        "DefaultDRRScheduler",
        "DefaultDRRSchedulerWithUserQuant",
        "DefaultPFScheduler"
    };

    for (const auto& type : scheduler_types) {
        Settings settings(
            1, "5G", 10, "1ms", "100ms", type,
            test_queue_configs, test_user_configs,
            test_bs_config, "urban", 10, 100, 50);

        auto scheduler = settings.get_scheduler_instance();
        EXPECT_NE(scheduler, nullptr) << "Failed for scheduler type: " << type;
    }

    // Проверка неверного типа планировщика
    Settings invalid_settings(
        1, "5G", 10, "1ms", "100ms", "InvalidScheduler",
        test_queue_configs, test_user_configs,
        test_bs_config, "urban", 10, 100, 50);

    EXPECT_EQ(invalid_settings.get_scheduler_instance(), nullptr);
}

TEST_F(SettingsTest, ConfigsHandling) {
    Settings settings(
        1, "5G", 10, "1ms", "100ms", "DefaultRRScheduler",
        test_queue_configs, test_user_configs,
        test_bs_config, "urban", 10, 100, 50);

    // Проверка доступа к конфигурациям по индексу
    auto queue0 = settings.get_queue_config(0);
    EXPECT_EQ(queue0.get_packet_count(), 100);

    auto user1 = settings.get_user_config(1);
    EXPECT_DOUBLE_EQ(user1.get_x(), 4.0);

    // Проверка получения всех конфигураций
    auto all_queues = settings.get_queue_configs();
    EXPECT_EQ(all_queues.size(), 2);

    auto all_users = settings.get_user_configs();
    EXPECT_EQ(all_users.size(), 2);
}

TEST_F(SettingsTest, CalculatedParameters) {
    Settings settings(
        1, "LTE", 10, "1ms", "10ms", "DefaultRRScheduler",
        test_queue_configs, test_user_configs,
        test_bs_config, "urban", 10, 100, 50);

    // Проверка вычисления TTI
    EXPECT_DOUBLE_EQ(settings.get_tti_value(), 0.001);

    // Проверка вычисления интервала синхронизации канала
    EXPECT_DOUBLE_EQ(settings.get_channel_sync_interval_value(), 0.010);

    // Проверка вычисления количества RB
    EXPECT_GT(settings.get_resource_block_per_tti_limit(), 0);

    // Проверка вычисления максимального размера пакета
    EXPECT_GT(settings.get_packet_size_limit(), 0);
}

TEST_F(SettingsTest, BoundaryValues) {
    // Пустые конфигурации
    std::vector<QueueConfig> empty_queues;
    std::vector<UserConfig> empty_users;
    
    // Проверка минимальных значений
    Settings min_settings(
        0, "", 0, "0ms", "0ms", "",
        empty_queues, empty_users,
        BSConfig(0, 0, 0, 0, 0, 0),
        "", 0, 0, 0);

    EXPECT_EQ(min_settings.get_launches(), 0);
    EXPECT_EQ(min_settings.get_base_cqi(), 0);
    EXPECT_EQ(min_settings.get_queue_count(), 0);
    EXPECT_EQ(min_settings.get_user_count(), 0);
    EXPECT_EQ(min_settings.get_total_packet_count(), 0);

    // Проверка максимальных значений
    Settings max_settings(
        INT_MAX, "LTE", 15, "10ms", "1s", "DefaultPFScheduler",
        {QueueConfig(INT_MAX, DBL_MAX, DBL_MAX)},
        {UserConfig(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, "", DBL_MAX, INT_MAX, DBL_MAX)},
        BSConfig(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, INT_MAX),
        "rural", INT_MAX, INT_MAX, INT_MAX);

    EXPECT_EQ(max_settings.get_launches(), INT_MAX);
    EXPECT_EQ(max_settings.get_total_packet_count(), INT_MAX);
    EXPECT_DOUBLE_EQ(max_settings.get_bandwidth(), DBL_MAX);
}