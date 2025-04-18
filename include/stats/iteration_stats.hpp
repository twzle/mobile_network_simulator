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
        int user_id,
        double processing_delay);

    void update_queue_time_stats(
        PacketQueueState queue_state,
        size_t queue_id,
        double tti_duration);
    void update_scheduler_time_stats(
        SchedulerState scheduler_state,
        double tti_duration);
    void update_scheduler_fairness_for_queues(
        int tti_count,
        double fairness_for_queues,
        bool is_valid);
    void update_scheduler_fairness_for_users(
        int tti_count,
        double fairness_for_users,
        bool is_valid);
    void update_scheduler_throughput(
        double throughput,
        bool is_valid);
    void update_scheduler_unused_resources(
        double unused_resources,
        bool is_valid);

    void evaluate(int queue_count, int user_count);

    void evaluate_queue_idle_time_stats();
    void evaluate_queue_wait_time_stats();
    void evaluate_queue_processing_time_stats();
    void evaluate_queue_total_time_stats();

    void evaluate_fairness_for_queues_stats();
    void evaluate_fairness_for_users_stats();

    void evaluate_user_delay_stats(int user_count);
    void evaluate_queue_delay_stats(int queue_count);
    void evaluate_scheduler_delay_stats();

    void evaluate_scheduling_stats();

    void evaluate_throughput_stats();
    void evaluate_unused_resources_stats();

    void release_memory_resources();

    void print();
    void print_user_delays();
    void print_queue_delays();

    void draw_delay_plot();
    void draw_scheduling_plot();

    int packet_count = 0; // Общее число обслуженных пакетов

    std::vector<double> scheduler_throughput; // За каждый TTI (Мбит/мс)
    double scheduler_average_throughput = 0;  // Среднее за весь период работы (Мбит/мс)

    std::vector<double> scheduler_unused_resources; // За каждый TTI (доля RB от максимума)
    double scheduler_average_unused_resources = 0;  // Среднее за весь период работы (доля RB от максимума)

    std::vector<std::pair<int, double> > scheduler_fairness_for_queues; // За несколько TTI пара (число TTI, результат)
    double scheduler_average_fairness_for_queues = 0;                   // Взвешенное среднее за весь период работы

    std::vector<std::pair<int, double> > scheduler_fairness_for_users; // За несколько TTI пара (число TTI, результат)
    double scheduler_average_fairness_for_users = 0;                   // Взвешенное среднее за весь период работы

    std::vector<PacketStats> packet_stats;
    std::map<int, double> queue_average_packet_processing_delay; // Средние задержки обработки пакетов по очередям (секунды)
    std::map<int, double> user_average_packet_processing_delay;  // Средние задержки обработки пакетов по пользователям (секунды)
    double scheduler_average_packet_processing_delay = 0;        // Общее среднее время задержки обработки пакетов (секунды)

    // Общее время работы (сумма processing, idle, wait)
    std::map<int, double> queue_total_time;
    double queue_average_total_time = 0; // Среднее время работы на очередь
    double scheduler_total_time = 0;     // Общее время работы планировщика

    // Время в которое производилось обслуживание не менее одного пакета любого размера
    std::map<int, double> queue_processing_time;
    double queue_average_processing_time = 0; // Среднее время облуживания на очередь
    double scheduler_processing_time = 0;     // Общее время обслуживания планировщика

    // Время в которое пакеты были готовы к обслуживанию, но не было ресурсов (вынужденный пропуск)
    std::map<int, double> queue_idle_time;
    double queue_average_idle_time = 0; // Среднее время простоя на очередь
    double scheduler_idle_time = 0;     // Общее время простоя планировщики

    // Время в которое пакетов не было в очередях или время их обслуживания не пришло (бездействие)
    std::map<int, double> queue_wait_time;
    double queue_average_wait_time = 0; // Среднее время бездействия на очередь
    double scheduler_wait_time = 0;     // Общее время бездействия планировщика
};