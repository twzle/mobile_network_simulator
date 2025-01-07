#pragma once

#include <map>
#include <stddef.h>
#include <cmath>

#include "const.hpp"

class TTIStats {
    public:
        TTIStats(size_t queue_count);

        void add_allocated_rb_to_queue(size_t queue_id, int rb_count);
        void add_allocated_rb_to_total(int rb_count);

        void calculate_fairness_for_queues();
        double get_fairness_for_queues();
        bool is_valid_fairness_for_queues();

    private:
        size_t queue_count; // Количество очередей
        int total_allocated_rb_count; // Общее число выделенных RB за TTI
        std::map<int, int> allocated_rb_count_per_queue; // Число выделенных RB по очередям
        double fairness_of_rb_allocation_for_queues; // Справедливость выделения RB относительно очередей
        bool _is_valid_fairness_for_queues; // Стоит ли учитывать справедливость в расчетах
};
