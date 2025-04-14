#pragma once

#include <map>
#include <stddef.h>
#include <cmath>

#include "core/user.hpp"
#include "config/standard_info.hpp"
#include "const.hpp"

class ThroughputStats
{
private:
    class UserThroughputStatus
    {
    public:
        int allocated_effective_data_size = 0;
        bool is_resource_candidate = false;
    };

    class QueueThroughputStatus
    {
    public:
        int allocated_effective_data_size = 0;
        bool is_resource_candidate = false;
    };

public:
    ThroughputStats();
    ThroughputStats(
        size_t queue_count, size_t user_count,
        double tti_duration);

    void initialize(
        int resource_block_limit_per_tti,
        size_t queue_count, size_t user_count,
        double tti_duration, int history_size_limit);
    void initialize_queue_stats();
    void initialize_user_stats();

    void reset();
    int get_current_history_size();
    void increment_current_history_size();
    bool is_history_size_limit_reached();

    void mark_queue_as_resource_candidate(size_t queue_id);
    void mark_user_as_resource_candidate(User *user);

    void add_allocated_effective_data_to_queue(
        size_t queue_id, int packet_size_in_bytes);
    void add_allocated_effective_data_to_user(
        User *user, int packet_size_in_bytes);
    void add_allocated_effective_data_to_total(
        int packet_size_in_bytes, int resource_blocks);

    void calculate_throughput_for_scheduler();
    double get_throughput_for_scheduler();
    bool is_valid_throughput_for_scheduler();

    void calculate_unused_resources_for_scheduler();
    double get_unused_resources_for_scheduler();
    bool is_valid_unused_resources_for_scheduler();

    int get_total_allocated_effective_data_size();
    int get_total_allocated_resource_blocks();
    int get_max_allocated_resource_blocks();
    double get_tti_duration();

    size_t get_user_count();
    size_t get_queue_count();

private:
    int history_size_limit;                  // Максимальный размер истории записи статистики (число TTI)
    int current_history_size;                // Текущий размер истории записи статистики (число TTI)
    int total_allocated_effective_data_size; // Общий размер полезных данных за TTI (бит)
    int total_allocated_resource_blocks;     // Общее число RB за TTI
    int max_allocated_resource_blocks;       // Максимально возможное число RB за TTI
    double tti_duration;                     // Интервал времени TTI (секунды)

    size_t queue_count;                                  // Общее количество очередей
    size_t candidate_queue_count;                        // Количество очередей нуждающихся в RB
    std::map<int, QueueThroughputStatus> queue_statuses; // Число выделенных RB по очередям

    size_t user_count;                                 // Общее количество пользователей
    size_t candidate_user_count;                       // Количество пользователей нуждающихся в RB
    std::map<int, UserThroughputStatus> user_statuses; // Число выделенных RB по пользователям

    double throughput_for_scheduler;         // Пропускная способность планировщика (Мбит/мс)
    bool _is_valid_throughput_for_scheduler; // Стоит ли учитывать пропускную сп-ность в расчетах

    double unused_resources_for_scheduler;         // Доля неиспользованных ресурсов планировщика (RB/RB_max)
    bool _is_valid_unused_resources_for_scheduler; // Стоит ли учитывать пропускную сп-ность в расчетах
};