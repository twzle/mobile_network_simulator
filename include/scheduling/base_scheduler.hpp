#pragma once

#include <map>
#include <vector>
#include <string>

#include "config/user_config.hpp"
#include "config/bs_config.hpp"
#include "config/tbs.hpp"
#include "core/state.hpp"
#include "core/packet_queue.hpp"
#include "core/user.hpp"
#include "core/base_station.hpp"
#include "core/channel.hpp"
#include "stats/iteration_stats.hpp"
#include "stats/fairness_stats.hpp"
#include "stats/throughput_stats.hpp"
#include "scheduling/scheduler_session.hpp"

class BaseScheduler
{
public:
    explicit BaseScheduler();
    virtual ~BaseScheduler() = default;

    // Основной метод запуска планировщика
    virtual void run() = 0;

    // Добавление очередей для обслуживания
    virtual void schedule(PacketQueue &&packet_queue) = 0;

    // Установка базовой станции
    void set_base_station(BSConfig bs_config);
    // Установка лимита ресурсных блоков на один TTI
    void set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit);
    // Установка длительности одного TTI
    void set_tti_duration(double tti_duration);
    // Установка длительности одного TTI
    void set_channel_sync_interval(double channel_sync_interval);
    // Установка базового CQI
    void set_base_cqi(uint8_t base_cqi);
    // Установка канала связи
    void set_channel(Channel channel);
    // Установка лимита пользователей
    void set_users_per_tti_limit(int users_per_tti_limit);
    // Инициализация статистики по TTI
    void set_tti_stats(
        int resource_block_limit_per_tti,
        size_t queue_count, size_t user_count,
        double tti_duration, int history_size_limit);

    // Подключение пользователей для обслуживания
    void set_users(
        std::vector<UserConfig> user_count,
        int throughput_history_size);
    User *get_user_ptr(int user_id);

    // Получение статистики выполнения
    IterationStats &get_stats();
    virtual void evaluate_stats() = 0;
    virtual void evaluate_fairness_stats(
        SchedulerState scheduler_state, bool force_update) = 0;
    virtual void evaluate_throughput_stats(bool force_update) = 0;

    int convert_packet_size_to_rb_number(User *user, int packet_size);

    virtual void sync_user_channels() = 0;

    void mark_as_resource_candidate(int queue, User* user);

    void save_processed_packet_stats(
        Packet &packet, 
        int packet_size_in_bytes, int packet_size_in_rb, 
        double current_time);

    int get_total_packets();

protected:
    int total_packets = 0;                      // Общее число пакетов для обслуживания
    double tti_duration = 0;                    // Длительность TTI в секундах
    double channel_sync_interval = 0;           // Интервал синхронизации канала в секундах
    uint8_t base_cqi = 0;                       // Базовый CQI
    int resource_block_effective_data_size = 0; // Размер полезных данных (байт) в одном RB
    int resource_blocks_per_tti = 0;            // Общее число RB на TTI
    int users_per_tti_limit = 0;                // Лимит пользователей обслуживаемых за TTI

    std::map<int, User> connected_users; // Подключенные пользователи (id -> ptr)
    BaseStation base_station;            // Базовая станция
    Channel channel;                     // Канал связи

    SchedulerSession session;         // Данные сессии работы планировщика
    IterationStats stats;             // Статистика с минимальным необходимым набором полей для дальнейших расчетов
    FairnessStats fairness_stats;     // Подробная статистика справедливости распределения ресурсов за указаное число TTI
    ThroughputStats throughput_stats; // Подробная статистика пропускной способности за указаное число TTI
};