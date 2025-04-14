#pragma once

#include <map>
#include <stddef.h>
#include <cmath>

#include "core/user.hpp"
#include "config/standard_info.hpp"
#include "const.hpp"

class FairnessStats
{
private:
    class UserFairnessStatus
    {
    public:
        int allocated_effective_data_size = 0;
        bool is_resource_candidate = false;
    };

    class QueueFairnessStatus
    {
    public:
        int allocated_effective_data_size = 0;
        bool is_resource_candidate = false;
    };

public:
    FairnessStats();
    FairnessStats(
        size_t queue_count, size_t user_count,
        double tti_duration);

    void initialize(
        size_t queue_count, size_t user_count,
        double tti_duration, int history_size_limit);
    void initialize_queue_stats();
    void initialize_user_stats();

    void reset();
    int get_current_history_size();
    int get_history_size_limit();
    void increment_current_history_size();
    bool is_history_size_limit_reached();

    void mark_queue_as_resource_candidate(size_t queue_id);
    void mark_user_as_resource_candidate(User *user);

    void add_allocated_effective_data_to_queue(
        size_t queue_id, int packet_size_in_bytes);
    void add_allocated_effective_data_to_user(
        User *user, int packet_size_in_bytes);
    void add_allocated_effective_data_to_total(
        int packet_size_in_bytes);

    void calculate_fairness_for_queues();
    double get_fairness_for_queues();
    bool is_valid_fairness_for_queues();

    void calculate_fairness_for_users();
    double get_fairness_for_users();
    bool is_valid_fairness_for_users();

    int get_total_allocated_effective_data_size();
    double get_tti_duration();

    size_t get_user_count();
    size_t get_queue_count();

private:
    int history_size_limit;                  // Максимальный размер истории записи статистики (число TTI)
    int current_history_size;                // Текущий размер истории записи статистики (число TTI)
    int total_allocated_effective_data_size; // Общий размер полезных данных за TTI (бит)
    double tti_duration;                     // Интервал времени TTI (секунды)

    size_t queue_count;                                      // Общее количество очередей
    size_t candidate_queue_count;                            // Количество очередей нуждающихся в RB
    std::map<int, QueueFairnessStatus> queue_statuses;       // Число выделенных RB по очередям
    double fairness_of_effective_data_allocation_for_queues; // Справедливость выделения RB относительно очередей
    bool _is_valid_fairness_for_queues;                      // Стоит ли учитывать справедливость в расчетах

    size_t user_count;                                      // Общее количество пользователей
    size_t candidate_user_count;                            // Количество пользователей нуждающихся в RB
    std::map<int, UserFairnessStatus> user_statuses;        // Число выделенных RB по пользователям
    double fairness_of_effective_data_allocation_for_users; // Справедливость выделения RB относительно пользователей
    bool _is_valid_fairness_for_users;                      // Стоит ли учитывать справедливость в расчетах
};