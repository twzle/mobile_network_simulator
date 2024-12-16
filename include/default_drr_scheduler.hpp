#pragma once

#include <map>
#include "packet_queue.hpp"
#include "execution_stats.hpp"

// Станадарты связи
enum class TTI
{
    LTE = 1,
};

// Размер TTI в секундах
extern std::map<TTI, double> tti_values;

class DefaultDRRScheduler
{
public:
    DefaultDRRScheduler();

    void run();
    void schedule(PacketQueue &&packet_queue);
    void set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit);

    void evaluate_stats();
    ExecutionStats &get_stats();

    int get_initial_queue();
    void set_initial_queue(int new_initial_queue_id);

    size_t get_relative_queue_id(size_t current_absolute_queue_id);

private:
    double tti_duration = 0;                   // Длительность TTI в секундах
    int resource_blocks_per_tti = 10;           // Общее число RB на TTI
    int current_initial_absolute_queue_id = 0; // Абсолютный (без смещения отн. 0) ID очереди с которой начинается распределение ресурсов RB
    std::vector<PacketQueue> scheduled_queues; // Обслуживаемые очереди

    double scheduling_duration = 0; // Длительность работы планировщика в
    int total_packets = 0;          // Общее число пакетов для обслуживания во всех очередях
    ExecutionStats stats;           // Статистика производительности планировщика
};