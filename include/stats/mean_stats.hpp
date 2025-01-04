#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>

#include "iteration_stats.hpp"

class MeanStats
{
public:
    MeanStats() = default;
    MeanStats(const MeanStats &) = delete;
    void calculate();
    void calculate_mean_values();
    void calculate_mean_delays();
    void calculate_mean_packet_scheduling_time();
    void calculate_mean_queue_processing_time();

    // Подсчет стандартного отклонения величины
    double calculate_standard_deviation_for_metric(
        const std::vector<double> &values,
        const double &mean);

    void calculate_execution_count_for_metric(
        const double &standard_deviation,
        const double &accuracy = 1);

    void collect_history();

    void show();
    void draw_delay_plot();
    void draw_scheduling_plot();
    void draw_queue_processing_time_plot();

    std::string write_yaml();

    double mean_total_time = 0;
    double common_total_time = 0;

    double mean_total_idle_time = 0;
    double common_total_idle_time = 0;

    double mean_total_skip_time = 0;
    double common_total_skip_time = 0;

    double mean_total_processing_time = 0;
    double common_total_processing_time = 0;

    double mean_total_wait_time = 0;
    double common_total_wait_time = 0;

    double mean_total_packet_count = 0;
    double common_total_packet_count = 0;

    std::vector<double> queue_mean_processing_time; // TODO: поменять на std::map<int, double>
    double total_mean_processing_time_by_scheduler = 0;

    std::vector<double> mean_delays_by_queue; // TODO: поменять на std::map<int, double>
    double total_mean_delay_by_scheduler = 0;
    double standard_devaition_delay_by_scheduler = 0;
    double execution_count_for_delay_credability = 0;

    std::vector<IterationStats> stats_array;

    // История ср. времени задержек обработки пакетов в очередях по запускам
    std::map<int, std::vector<double>> mean_delay_by_queue_history;
    // История ср. задержек обработки пакетов в планировщике (во всех очередях) по запускам
    std::vector<double> mean_delay_by_scheduler_history;

    // История ср. времени работы каждой очереди по запускам
    std::map<int, std::vector<double>> processing_time_by_queue_history;
    // История ср. времени работы планировщика по запускам
    std::vector<double> mean_processing_time_by_scheduler_history;
};