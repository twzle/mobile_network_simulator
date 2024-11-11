#pragma once

#include <map>
#include <vector>
#include "packet_stats.hpp"

class ExecutionStats {
    public:
        ExecutionStats() = default;

        void print();
        void evaluate();
        void evaluate_queue_processing_time_stats();
        void evaluate_delay_stats();
        void evaluate_scheduling_stats();
        void draw_delay_plot();
        void draw_scheduling_plot();

        double total_time = 0; // Общее время работы планировщика в мс
        double total_skip_time = 0; // Общее время простоя планировщика в мс
        double total_processing_time = 0; // Общее время обслуживания пакетов в мс
        int packet_count = 0; // Общее число обслуженных пакетов
        int retried_packet_count = 0; // Число обслуженных пакетов не с первого раза
        int lost_packet_count = 0; // Число потерянных пакетов
        std::map<int, std::vector<PacketStats>> queue_stats;

        std::vector<double> average_delay_by_queue; // Средние задержки обработки пакетов
        double average_delay = 0;
        
        std::map<int, double> queue_processing_time;
        double average_queue_processing_time = 0; // Среднее время работы всех очередей
};