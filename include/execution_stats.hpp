#pragma once

#include <map>
#include <vector>
#include "packet_stats.hpp"

class ExecutionStats {
    public:
        ExecutionStats();

        void summarize();
        void evaluate();
        void evaluate_delay_stats();
        void evaluate_scheduling_stats();
        void draw_delay_plot();
        void draw_scheduling_plot();

        double total_time; // Общее время работы планировщика в мс
        double total_skip_time; // Общее время простоя планировщика в мс
        double total_processing_time; // Общее время обслуживания пакетов в мс
        int packet_count; // Общее число обслуженных пакетов
        int retried_packet_count; // Число обслуженных пакетов не с первого раза
        int lost_packet_count; // Число потерянных пакетов
        std::map<int, std::vector<PacketStats>> queue_stats;

        std::vector<float> average_delays; // Средние задержки обработки пакетов
        std::map<int, std::vector<int>> packets_scheduled_by_ms; // Количество пакетов пришедших в каждый момент времени по очередям
};