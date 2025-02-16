#pragma once

#include <map>
#include <stddef.h>
#include <cmath>

#include "core/user.hpp"
#include "const.hpp"
#include "standard_info.hpp"

class TTIStats
{
private:
    class UserStatus
    {
    public:
        int allocated_effective_data_size = 0;
        bool is_resource_candidate = false;
    };

    class QueueStatus
    {
    public:
        int allocated_effective_data_size = 0;
        bool is_resource_candidate = false;
    };

public:
    TTIStats(
        std::string standard_name,
        size_t queue_count, size_t user_count,
        double tti_duration);

    void initialize_queue_stats();
    void initialize_user_stats();

    void mark_queue_as_resource_candidate(size_t queue_id);
    void mark_user_as_resource_candidate(User *user);

    void add_allocated_effective_data_to_queue(
        User *user, size_t queue_id, int rb_count);
    void add_allocated_effective_data_to_user(
        User *user, int rb_count);
    void add_allocated_effective_data_to_total(
        User *user, int rb_count);

    void calculate_fairness_for_queues();
    double get_fairness_for_queues();
    bool is_valid_fairness_for_queues();

    void calculate_fairness_for_users();
    double get_fairness_for_users();
    bool is_valid_fairness_for_users();

    void calculate_throughput_for_scheduler();
    double get_throughput_for_scheduler();
    bool is_valid_throughput_for_scheduler();

private:
    std::string standard_name = "LTE"; // Название cтандарта

    int total_allocated_effective_data_size; // Общий размер полезных данных за TTI (бит)
    double tti_duration;                     // Интервал времени TTI (секунды)

    size_t queue_count;                                      // Общее количество очередей
    size_t candidate_queue_count;                            // Количество очередей нуждающихся в RB
    std::map<int, QueueStatus> queue_statuses;               // Число выделенных RB по очередям
    double fairness_of_effective_data_allocation_for_queues; // Справедливость выделения RB относительно очередей
    bool _is_valid_fairness_for_queues;                      // Стоит ли учитывать справедливость в расчетах

    size_t user_count;                                      // Общее количество пользователей
    size_t candidate_user_count;                            // Количество пользователей нуждающихся в RB
    std::map<int, UserStatus> user_statuses;                // Число выделенных RB по пользователям
    double fairness_of_effective_data_allocation_for_users; // Справедливость выделения RB относительно пользователей
    bool _is_valid_fairness_for_users;                      // Стоит ли учитывать справедливость в расчетах

    double throughput_for_scheduler;         // Пропускная способность планировщиках (Кбайт/мс)
    bool _is_valid_throughput_for_scheduler; // Стоит ли учитывать пропускную сп-ность в расчетах
};
