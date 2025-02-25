#pragma once

#include <stdexcept>
#include <memory>
#include <vector>
#include <algorithm>

#include "scheduling/base_rr_scheduler.hpp"
#include "scheduling/default_rr_scheduler.hpp"
#include "scheduling/fixed_drr_scheduler.hpp"
#include "scheduling/cyclic_drr_scheduler.hpp"
#include "scheduling/default_drr_scheduler.hpp"
#include "standard_info.hpp"
#include "user_config.hpp"
#include "bs_config.hpp"
#include "const.hpp"

class Settings
{
public:
    Settings() = default;
    Settings(
        int launches,
        std::string standard_type,
        uint8_t cqi,
        std::string tti_duration,
        std::string channel_sync_interval,
        std::string scheduler_type,
        double bandwidth,
        int packet_count,
        int packet_size,
        int queue_count,
        double queue_quant,
        int queue_limit,
        int user_count,
        double time_lambda,
        std::vector<UserConfig> user_configs,
        BSConfig bs_config);

    void validate();

    int get_launches();
    std::string get_standard_type();
    uint8_t get_cqi();
    std::string get_tti_duration();
    std::string get_channel_sync_interval();
    std::unique_ptr<BaseRRScheduler> get_scheduler_instance();
    double get_bandwidth();
    double get_time_lambda();
    int get_packet_count();
    int get_packet_size();
    int get_queue_count();
    int get_user_count();
    double get_queue_quant();
    int get_queue_limit();
    BSConfig get_bs_config();
    std::vector<UserConfig> get_user_configs();
    int get_resource_block_per_tti_limit();
    int get_packet_size_limit();

private:
    int launches;                         // Количество повторов
    std::string standard_type;            // Стандарт передачи данных
    uint8_t cqi;                          // CQI
    std::string tti_duration;             // Длительность TTI
    std::string channel_sync_interval;    // Интервал синхронизации канала
    std::string scheduler_type;           // Тип планировщика
    double bandwidth;                     // Полоса пропускания в МГц
    int packet_count;                     // Количество пакетов
    int packet_size;                      // Размеры пакетов
    int queue_count;                      // Количество очередей
    double queue_quant;                   // Квант времени очередей
    int queue_limit;                      // Размеры очередей
    int user_count;                       // Количество пользователей
    double time_lambda;                   // Частота прихода пакетов в единицу времени
    std::vector<UserConfig> user_configs; // Конфигурации пользователей
    BSConfig bs_config;                   // Конфигурация базовой станции
};