#pragma once

#include <map>
#include <stddef.h>
#include <cmath>

#include "core/user.hpp"
#include "const.hpp"

class TTIStats
{
public:
    TTIStats(size_t queue_count, size_t user_count);

    void initialize_queue_stats();
    void initialize_user_stats();

    void add_allocated_rb_to_queue(size_t queue_id, int rb_count);
    void add_allocated_rb_to_user(User* user, int rb_count);
    void add_allocated_rb_to_total(int rb_count);

    void calculate_fairness_for_queues();
    double get_fairness_for_queues();
    bool is_valid_fairness_for_queues();

    void calculate_fairness_for_users();
    double get_fairness_for_users();
    bool is_valid_fairness_for_users();

private:
    int total_allocated_rb_count; // Общее число выделенных RB за TTI

    size_t queue_count;                              // Количество очередей
    std::map<int, int> allocated_rb_count_per_queue; // Число выделенных RB по очередям
    double fairness_of_rb_allocation_for_queues;     // Справедливость выделения RB относительно очередей
    bool _is_valid_fairness_for_queues;              // Стоит ли учитывать справедливость в расчетах

    size_t user_count;                              // Количество пользователей
    std::map<int, int> allocated_rb_count_per_user; // Число выделенных RB по пользователям
    double fairness_of_rb_allocation_for_users;     // Справедливость выделения RB относительно пользователей
    bool _is_valid_fairness_for_users;              // Стоит ли учитывать справедливость в расчетах
};
