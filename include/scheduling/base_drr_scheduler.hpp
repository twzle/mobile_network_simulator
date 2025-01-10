#pragma once

#include <map>
#include <vector>
#include <string>

#include "core/state.hpp"
#include "core/packet_queue.hpp"
#include "core/user.hpp"
#include "stats/iteration_stats.hpp"
#include "stats/tti_stats.hpp"
#include "scheduling/scheduler_session.hpp"

class BaseDRRScheduler
{
public:
    explicit BaseDRRScheduler(double tti);
    virtual ~BaseDRRScheduler() = default;

    // Основной метод запуска планировщика
    virtual void run() = 0;

    // Добавление очередей для обслуживания
    void schedule(PacketQueue &&packet_queue);

    // Подключение пользователей для обслуживания
    void connect_users(int user_count);
    User* get_user_ptr(int user_id);

    // Установка лимита ресурсных блоков на один TTI
    void set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit);

    // Получение статистики выполнения
    IterationStats &get_stats();
    void evaluate_stats();

    // Методы управления начальной очередью
    virtual size_t get_initial_queue();
    virtual void set_initial_queue(size_t new_initial_queue_id);

protected:
    // Метод для получения относительного индекса очереди (может быть переопределен)
    virtual size_t get_relative_queue_id(size_t current_absolute_queue_id);

protected:
    int total_packets = 0;                        // Общее число пакетов для обслуживания
    double tti_duration = 0;                      // Длительность TTI в секундах
    int resource_blocks_per_tti = 0;              // Общее число RB на TTI
    size_t current_initial_absolute_queue_id = 0; // Абсолютный ID начальной очереди
    
    std::vector<PacketQueue> scheduled_queues;    // Очереди для обслуживания
    std::map<int, User> connected_users; // Подключенные пользователи

    SchedulerSession session; // Данные сессии работы планировщика
    IterationStats stats;     // Статистика с минимальным необходимым набором полей для дальнейших расчетов
};