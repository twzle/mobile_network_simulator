#pragma once

#include <vector>
#include "execution_stats.hpp"

class AverageStats {
    public:
        AverageStats() = default;
        void calculate();
        void calculate_average_values();
        void calculate_average_delays();
        void calculate_average_packet_scheduling_time();
        void show();
        void draw_delay_plot();
        void draw_scheduling_plot();

        float average_total_time = 0;
        int common_total_time = 0;

        float average_total_idle_time = 0;
        int common_total_idle_time = 0;

        float average_total_skip_time = 0;
        int common_total_skip_time = 0;

        float average_total_processing_time = 0;
        int common_total_processing_time = 0;

        float average_total_packet_count = 0;
        int common_total_packet_count = 0;

        float average_total_lost_packet_count = 0;
        int common_total_lost_packet_count = 0;
    
        float average_total_retried_packet_count = 0;
        int common_total_retried_packet_count = 0;

        std::vector<float> average_delays;
        std::map<int, std::vector<float>> average_packets_scheduled_by_ms;

        std::vector<ExecutionStats> stats_array;

};