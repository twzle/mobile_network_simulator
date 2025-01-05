#pragma once

#include <map>
#include <vector>
#include <string>

#include "core/state.hpp"
#include "core/packet_queue.hpp"
#include "stats/iteration_stats.hpp"

class BaseDRRScheduler
{
public:
    explicit BaseDRRScheduler(double tti);
    virtual ~BaseDRRScheduler() = default;

    // Основной метод запуска планировщика
    virtual void run() = 0;

    // Добавление очередей для обслуживания
    void schedule(PacketQueue &&packet_queue);

    void set_scheduling_start_time(double time);
    void set_scheduling_end_time(double time);
    double get_scheduling_start_time();
    double get_scheduling_end_time();

    // Установка лимита ресурсных блоков на один TTI
    void set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit);

    void increment_processed_packet_count(int increment);

    // Получение статистики выполнения
    IterationStats &get_stats();
    void evaluate_stats();

    // Методы управления начальной очередью
    virtual size_t get_initial_queue();
    virtual void set_initial_queue(size_t new_initial_queue_id);

protected:
    double tti_duration = 0;                      // Длительность TTI в секундах
    int resource_blocks_per_tti = 0;              // Общее число RB на TTI
    size_t current_initial_absolute_queue_id = 0; // Абсолютный ID начальной очереди
    std::vector<PacketQueue> scheduled_queues;    // Очереди для обслуживания

    // TODO: Внести в IterationStats
    double scheduling_start = 0;    // Начало работы планировщика
    double scheduling_end = 0;      // Конец работы планировщика
    double scheduling_duration = 0; // Общая длительность работы планировщика
    int processed_packets = 0;      // Общее число обслуженных пакетов
    int total_packets = 0;          // Общее число пакетов для обслуживания

    IterationStats stats; // Статистика производительности планировщика

    // Метод для получения относительного индекса очереди (может быть переопределен)
    virtual size_t get_relative_queue_id(size_t current_absolute_queue_id);
};
