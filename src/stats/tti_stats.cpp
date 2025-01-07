#include "stats/tti_stats.hpp"

TTIStats::TTIStats(size_t queue_count) : 
queue_count(queue_count), total_allocated_rb_count(0), 
fairness_of_rb_allocation_for_queues(0), _is_valid_fairness_for_queues(false)
{
    for (size_t queue_id = 0; queue_id < queue_count; ++queue_id)
    {
        allocated_rb_count_per_queue[queue_id] = 0;
    }
}

void TTIStats::add_allocated_rb_to_queue(size_t queue_id, int rb_count)
{
    allocated_rb_count_per_queue[queue_id] += rb_count;
}

void TTIStats::add_allocated_rb_to_total(int rb_count)
{
    total_allocated_rb_count += rb_count;
}

/*
Справедливость относительно очередей

Общая формула: JFI = ((sum(RB_i))^2)/(N * sum(RB_i^2))
RB_i - кол-во RB выделенных очереди i,
N - кол-во очередей
*/

void TTIStats::calculate_fairness_for_queues()
{
    _is_valid_fairness_for_queues = false;

    if (total_allocated_rb_count > 0)
    {
        // sum(RB_i))^2
        int squared_sum_of_allocated_rb_count = std::pow(total_allocated_rb_count, 2);

        // sum(RB_i^2)
        int sum_of_squared_allocated_rb_count_by_queue = 0;
        for (auto &queue : allocated_rb_count_per_queue)
        {
            int squared_allocated_rb_count_by_queue = queue.second;
            sum_of_squared_allocated_rb_count_by_queue += 
                squared_allocated_rb_count_by_queue;
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_rb_allocation_for_queues = 
            squared_sum_of_allocated_rb_count / 
            (queue_count * sum_of_squared_allocated_rb_count_by_queue);

        _is_valid_fairness_for_queues = true;
    }
}

double TTIStats::get_fairness_for_queues(){
    return fairness_of_rb_allocation_for_queues;
}

bool TTIStats::is_valid_fairness_for_queues(){
    return _is_valid_fairness_for_queues;
}