#pragma once

#include <map>
#include <vector>
#include <stddef.h>
#include "packet_stats.hpp"

class ExecutionStats {
    public:
        ExecutionStats() = default;
        ExecutionStats(const ExecutionStats&) = delete;
        ExecutionStats(ExecutionStats&&) = default;

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


        double get_queue_total_time(size_t queue_id);
        void set_queue_total_time(
            size_t queue_id, 
            double total_processing_time);

        void add_queue_packet_stats(
            size_t queue_id, 
            double scheduled_at, 
            double processed_at);

        void update_queue_time_stats(
            int allocated_resource_blocks,
            size_t queue_id, 
            double tti_duration);
        void update_scheduler_time_stats(
            int allocated_resource_blocks,
            double tti_duration);
        
        void evaluate();
        void evaluate_values();
        void evaluate_queue_total_time_stats();
        void evaluate_queue_processing_time_stats();
        void evaluate_queue_idle_time_stats();
        void evaluate_delay_stats();
        void evaluate_scheduling_stats();

        void release_memory_resources();

        void print();
        void draw_delay_plot();
        void draw_scheduling_plot();


        int packet_count = 0; // Общее число обслуженных пакетов
        int retried_packet_count = 0; // Число обслуженных пакетов не с первого раза
        int lost_packet_count = 0; // Число потерянных пакетов
        std::map<int, std::vector<PacketStats>> queue_stats;

        std::vector<double> average_delay_by_queue; // Средние задержки обработки пакетов
        double average_delay_by_scheduler = 0;
        
        // Время прошедшее для обслуживания всех пакетов в очереди относительно начала планирования
        std::map<int, double> queue_total_time;
        double scheduler_average_total_time = 0; // Среднее время работы всех очередей
        double scheduler_total_time = 0; // Общее время работы планировщика в мс
        

        // Время учитывающее только моменты в которые был обслужен хотя бы 1 пакет
        std::map<int, double> queue_processing_time;
        double scheduler_average_processing_time = 0; // Среднее время работы всех очередей
        double scheduler_processing_time = 0; // Общее время работы планировщика в которое хотя бы 1 пакет был обслужен

        // Время учитывающее только моменты в которые ни 1 пакет не был обслужен
        std::map<int, double> queue_idle_time;
        double scheduler_average_idle_time = 0; // Среднее время работы всех очередей
        double scheduler_idle_time = 0; // Общее время работы планировщика в которое ни 1 пакет не был обслужен 
};