#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>

#include "config/standard_info.hpp"
#include "config/tbs.hpp"
#include "iteration_stats.hpp"


// Средняя статистика за все итерации моделирования 
class MeanStats
{
public:
    MeanStats() = default;
    MeanStats(const MeanStats &) = delete;
    void calculate(double bandwidth);
    void calculate_mean_values();
    void calculate_mean_queue_packet_processing_delays();
    void calculate_mean_user_packet_processing_delays();
    void calculate_mean_scheduler_packet_processing_delay();
    void calculate_mean_packet_scheduling_time();
    void calculate_mean_queue_processing_time();
    void calculate_max_scheduler_throughput(double bandwidth);

    // Подсчет стандартного отклонения величины
    double calculate_standard_deviation_for_metric(
        const std::vector<double> &values,
        const double &mean);

    void calculate_execution_count_for_metric(
        const double &standard_deviation,
        const double &accuracy);

    void calculate_confidence_interval(
        const std::vector<double> &values,
        const double &mean,
        const double &accuracy = 1);

    void evaluate_confidence_intervals();
    void evaluate_confidence_queue_packet_processing_delay_intervals();
    void evaluate_confidence_user_packet_processing_delay_intervals();

    void init_history();
    void init_queue_processing_delay_history();

    void collect_history();
    void collect_queue_packet_processing_delay_history();
    void collect_user_packet_processing_delay_history();

    void show();
    void show_queue_delays();
    void show_user_delays();

    void draw_delay_plot();
    void draw_scheduling_plot();
    void draw_queue_processing_time_plot();

    std::string write_yaml();

    // История среднего суммарного времени работы планировщика по итерациям
    std::vector<double> scheduler_total_time_history;
    double mean_scheduler_total_time = 0;
    double common_scheduler_total_time = 0;

    // История среднего суммарного времени работы планировщика по итерациям
    std::vector<double> scheduler_idle_time_history;
    double mean_scheduler_idle_time = 0;
    double common_scheduler_idle_time = 0;

    // История среднего суммарного времени работы планировщика по итерациям
    std::vector<double> scheduler_processing_time_history;
    double mean_scheduler_processing_time = 0;
    double common_scheduler_processing_time = 0;

    // История среднего суммарного времени работы планировщика по итерациям
    std::vector<double> scheduler_wait_time_history;
    double mean_scheduler_wait_time = 0;
    double common_scheduler_wait_time = 0;

    // История средней суммарной справедливости распределения RB между очередями по итерациям
    std::vector<double> scheduler_fairness_for_queues_history;
    double mean_scheduler_fairness_for_queues = 0;
    double common_scheduler_fairness_for_queues = 0;

    // История средней суммарной справедливости распределения RB между пользователями по итерациям
    std::vector<double> scheduler_fairness_for_users_history;
    double mean_scheduler_fairness_for_users = 0;
    double common_scheduler_fairness_for_users = 0;

    // История средней суммарной пропускной способности по итерациям
    std::vector<double> scheduler_throughput_history;
    double mean_scheduler_throughput = 0;   // (Мбит/мс)
    double common_scheduler_throughput = 0; // (Мбит/мс)
    double max_scheduler_throughput = 0; // (Мбит/мс)

    double mean_scheduler_packet_count = 0;
    double common_scheduler_packet_count = 0;

    std::vector<double> queue_mean_processing_time; // TODO: поменять на std::map<int, double>
    double total_mean_processing_time_by_scheduler = 0;

    // История ср. времени задержек обработки пакетов в очередям по итерациям
    std::map<int, std::vector<double> > queue_packet_processing_delay_history;
    std::map<int, double> mean_queue_packet_processing_delays;
    // История ср. времени задержек обработки пакетов по пользователям по итерациям
    std::map<int, std::vector<double> > user_packet_processing_delay_history;
    std::map<int, double> mean_user_packet_processing_delays;
    // История ср. задержек обработки пакетов в планировщике по итерациям
    std::vector<double> scheduler_packet_processing_delay_history;
    double mean_scheduler_packet_processing_delay = 0;

    std::vector<IterationStats> stats_array;
};