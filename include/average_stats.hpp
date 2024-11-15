#pragma once

#include <vector>
#include <string>
#include "execution_stats.hpp"

class AverageStats {
    public:
        AverageStats() = default;
        void calculate();
        void calculate_average_values();
        void calculate_average_delays();
        void calculate_average_packet_scheduling_time();
        void calculate_average_queue_processing_time();

        void calculate_standard_deviation_for_delay();
        void calculate_execution_count_for_delay();

        void show();
        void draw_delay_plot();
        void draw_scheduling_plot();
        void draw_queue_processing_time_plot();

        std::string write_yaml();

        double average_total_time = 0;
        double common_total_time = 0;

        double average_total_idle_time = 0;
        double common_total_idle_time = 0;

        double average_total_skip_time = 0;
        double common_total_skip_time = 0;

        double average_total_processing_time = 0;
        double common_total_processing_time = 0;

        double average_total_packet_count = 0;
        double common_total_packet_count = 0;

        double average_total_lost_packet_count = 0;
        double common_total_lost_packet_count = 0;
    
        double average_total_retried_packet_count = 0;
        double common_total_retried_packet_count = 0;

        std::vector<double> average_processing_time_by_queue;
        double total_average_processing_time_by_scheduler = 0;

        std::vector<double> average_delays_by_queue;
        double total_average_delay_by_scheduler = 0;
        double standard_devaition_delay_by_scheduler = 0;
        double execution_count_for_delay_credability = 0;

        std::vector<ExecutionStats> stats_array;

};