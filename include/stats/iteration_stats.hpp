#pragma once

#include <map>
#include <vector>
#include <stddef.h>
#include <iostream>

#include "core/time_generator.hpp"
#include "core/state.hpp"
#include "packet_stats.hpp"

class IterationStats
{
public:
    IterationStats() = default;
    IterationStats(const IterationStats &) = delete;
    IterationStats(IterationStats &&) = default;

    double get_queue_processing_time(size_t queue_id);
    void set_queue_processing_time(
        size_t queue_id,
        double processing_time);
    void increment_queue_processing_time(
        size_t queue_id,
        double tti_duration);
    void increment_scheduler_processing_time(
        double tti_duration);

    double get_queue_idle_time(size_t queue_id);
    void set_queue_idle_time(
        size_t queue_id,
        double idle_time);
    void increment_queue_idle_time(
        size_t queue_id,
        double tti_duration);
    void increment_scheduler_idle_time(
        double tti_duration);

    double get_queue_wait_time(size_t queue_id);
    void set_queue_wait_time(
        size_t queue_id,
        double wait_time);
    void increment_queue_wait_time(
        size_t queue_id,
        double tti_duration);
    void increment_scheduler_wait_time(
        double tti_duration);

    double get_queue_total_time(size_t queue_id);
    void set_queue_total_time(
        size_t queue_id,
        double total_processing_time);

    void add_queue_packet_stats(
        size_t queue_id,
        double scheduled_at,
        double processed_at);

    void update_queue_time_stats(
        PacketQueueState queue_state,
        size_t queue_id,
        double tti_duration);
    void update_scheduler_time_stats(
        SchedulerState scheduler_state,
        double tti_duration);

    void evaluate();

    void evaluate_queue_idle_time_stats();
    void evaluate_queue_wait_time_stats();
    void evaluate_queue_processing_time_stats();
    void evaluate_queue_total_time_stats();

    void evaluate_delay_stats();
    void evaluate_scheduling_stats();

    void release_memory_resources();

    void print();
    void draw_delay_plot();
    void draw_scheduling_plot();

    int packet_count = 0; // Общее число обслуженных пакетов
    std::vector<PacketStats> queue_stats;

    std::map<int, double> queue_average_delay; // Средние задержки обработки пакетов
    double scheduler_average_delay = 0;

    // Общее время работы (сумма processing, idle, wait)
    std::map<int, double> queue_total_time;
    double queue_average_total_time = 0; // Среднее время работы на очередь
    double scheduler_total_time = 0;         // Общее время работы планировщика

    // Время в которое производилось обслуживание не менее одного пакета любого размера
    std::map<int, double> queue_processing_time;
    double queue_average_processing_time = 0; // Среднее время облуживания на очередь
    double scheduler_processing_time = 0;         // Общее время обслуживания планировщика

    // Время в которое пакеты были готовы к обслуживанию, но не было ресурсов (вынужденный пропуск)
    std::map<int, double> queue_idle_time;
    double queue_average_idle_time = 0; // Среднее время простоя на очередь
    double scheduler_idle_time = 0;         // Общее время простоя планировщики

    // Время в которое пакетов не было в очередях или время их обслуживания не пришло (бездействие)
    std::map<int, double> queue_wait_time;
    double queue_average_wait_time = 0; // Среднее время бездействия на очередь
    double scheduler_wait_time = 0;         // Общее время бездействия планировщика
};