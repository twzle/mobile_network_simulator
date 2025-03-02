#pragma once

#include <map>
#include <vector>
#include <string>

#include "user_config.hpp"
#include "bs_config.hpp"
#include "core/state.hpp"
#include "core/packet_queue.hpp"
#include "core/user.hpp"
#include "core/base_station.hpp"
#include "core/channel.hpp"
#include "stats/iteration_stats.hpp"
#include "stats/tti_stats.hpp"
#include "scheduling/scheduler_session.hpp"

class BaseRRScheduler
{
public:
    explicit BaseRRScheduler();
    virtual ~BaseRRScheduler() = default;

    // Основной метод запуска планировщика
    virtual void run() = 0;

    // Добавление очередей для обслуживания
    void schedule(PacketQueue &&packet_queue);

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

    // Подключение пользователей для обслуживания
    void set_users(std::vector<UserConfig> user_count);
    User *get_user_ptr(int user_id);

    // Получение статистики выполнения
    IterationStats &get_stats();
    void evaluate_stats();

    // Методы управления начальной очередью
    virtual size_t get_initial_queue();
    virtual void set_initial_queue(size_t new_initial_queue_id);

protected:
    // Метод для получения относительного индекса очереди (может быть переопределен)
    virtual size_t get_relative_queue_id(size_t current_absolute_queue_id);

    void check_queue_remaining_scheduled_packets(
        PacketQueue &queue,
        double current_time,
        TTIStats &tti_stats);

    int convert_packet_size_to_rb_number(User *user, int packet_size);
    void sync_user_channels();

protected:
    int total_packets = 0;                        // Общее число пакетов для обслуживания
    double tti_duration = 0;                      // Длительность TTI в секундах
    double channel_sync_interval = 0;             // Интервал синхронизации канала в секундах
    uint8_t base_cqi = 0;                         // Базовый CQI
    int resource_block_effective_data_size = 0;   // Размер полезных данных (байт) в одном RB
    int resource_blocks_per_tti = 0;              // Общее число RB на TTI
    size_t current_initial_absolute_queue_id = 0; // Абсолютный ID начальной очереди

    bool check_start_pos = true;

    std::vector<PacketQueue> scheduled_queues; // Очереди для обслуживания
    std::map<int, User> connected_users;       // Подключенные пользователи
    BaseStation base_station;                  // Базовая станция
    Channel channel;                           // Канал связи

    SchedulerSession session; // Данные сессии работы планировщика
    IterationStats stats;     // Статистика с минимальным необходимым набором полей для дальнейших расчетов
};