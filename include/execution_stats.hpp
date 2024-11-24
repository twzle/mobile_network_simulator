#pragma once

#include <map>
#include <vector>
#include "packet_stats.hpp"

class ExecutionStats {
    public:
        ExecutionStats() = default;
        ExecutionStats(const ExecutionStats&) = delete;
        ExecutionStats(ExecutionStats&&) = default;

        void print();
        void evaluate();
        void evaluate_values();
        void evaluate_queue_processing_time_stats();
        void evaluate_delay_stats();
        void evaluate_scheduling_stats();

        void release_memory_resources();

        void draw_delay_plot();
        void draw_scheduling_plot();

        double total_time = 0; // Общее время работы планировщика в мс
        double total_skip_time = 0; // Общее время потраченное на переключение очередей при нехватке дефицита в мс
        double total_idle_time = 0; // Общее время работы планировщика без учета обслуживания пакетов в мс
        double total_processing_time = 0; // Общее время обслуживания пакетов в мс
        int packet_count = 0; // Общее число обслуженных пакетов
        int retried_packet_count = 0; // Число обслуженных пакетов не с первого раза
        int lost_packet_count = 0; // Число потерянных пакетов
        std::map<int, std::vector<PacketStats>> queue_stats;

        std::vector<double> average_delay_by_queue; // Средние задержки обработки пакетов
        double average_delay_by_scheduler = 0;
        
        std::map<int, double> processing_time_by_queue;
        double average_processing_time_by_scheduler = 0; // Среднее время работы всех очередей
};