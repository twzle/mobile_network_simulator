#pragma once

#include <stdexcept>
#include <memory>
#include <vector>
#include <algorithm>
#include <sstream>

#include "scheduling/base_scheduler.hpp"
#include "scheduling/RR/base_rr_scheduler.hpp"
#include "scheduling/PF/base_pf_scheduler.hpp"
#include "scheduling/RR/default_rr_scheduler.hpp"
#include "scheduling/DRR/fixed_drr_scheduler.hpp"
#include "scheduling/DRR/cyclic_drr_scheduler.hpp"
#include "scheduling/DRR/default_drr_scheduler.hpp"
#include "scheduling/DRR/fixed_drr_scheduler_with_user_quant.hpp"
#include "scheduling/DRR/cyclic_drr_scheduler_with_user_quant.hpp"
#include "scheduling/DRR/default_drr_scheduler_with_user_quant.hpp"
#include "core/channel.hpp"
#include "config/standard_info.hpp"
#include "config/user_config.hpp"
#include "config/queue_config.hpp"
#include "config/bs_config.hpp"
#include "config/tbs.hpp"
#include "const.hpp"

class Settings
{
public:
    Settings();
    Settings(
        int launches,
        std::string standard_type,
        uint8_t base_cqi,
        std::string tti_duration,
        std::string channel_sync_interval,
        std::string scheduler_type,
        std::vector<QueueConfig> queue_configs,
        std::vector<UserConfig> user_configs,
        BSConfig bs_config,
        std::string area_type,
        int users_per_tti_limit,
        int throughput_history_size,
        int fairness_history_size);

    void validate();
    void validate_scheduler_specific_parameters();

    int get_launches();
    std::string get_standard_type();
    uint8_t get_base_cqi();
    double get_tti_value();
    std::string get_channel_sync_interval();
    double get_channel_sync_interval_value();
    std::unique_ptr<BaseScheduler> get_scheduler_instance();
    std::string get_scheduler_name();
    double get_bandwidth();
    int get_total_packet_count();
    int get_packet_size();
    int get_queue_count();
    int get_user_count();
    double get_queue_quant();
    int get_queue_limit();
    BSConfig get_bs_config();
    std::vector<QueueConfig> get_queue_configs();
    QueueConfig get_queue_config(int queue_id);
    std::vector<UserConfig> get_user_configs();
    UserConfig get_user_config(int user_id);
    int get_resource_block_per_tti_limit();
    int get_packet_size_limit();
    double get_carrier_frequency();
    int get_bs_transmission_power();
    std::string get_area_type();
    int get_users_per_tti_limit();
    int get_throughput_history_size();
    int get_fairness_history_size();

private:
    int launches;                           // Количество повторов
    std::string standard_type;              // Стандарт передачи данных
    uint8_t base_cqi;                       // CQI
    std::string tti_duration;               // Длительность TTI
    std::string channel_sync_interval;      // Интервал синхронизации канала
    std::string scheduler_type;             // Тип планировщика
    int total_packet_count;                 // Количество пакетов во всех очередях
    int queue_count;                        // Количество очередей
    std::vector<QueueConfig> queue_configs; // Конфигурации очередей
    int user_count;                         // Количество пользователей
    std::vector<UserConfig> user_configs;   // Конфигурации пользователей
    BSConfig bs_config;                     // Конфигурация базовой станции
    std::string area_type;                  // Тип местности
    int users_per_tti_limit;                // Лимит обслуживаемых пользователей за TTI
    int throughput_history_size;            // Размер истории пропускной способности (PF)
    int fairness_history_size;              // Размер истории справедливости распределения ресурсов (число TTI)
};

Settings load_settings_from_yaml(const std::string &filename);