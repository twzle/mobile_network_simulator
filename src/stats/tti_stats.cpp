#include "stats/tti_stats.hpp"

TTIStats::TTIStats(
    size_t queue_count, size_t user_count,
    double tti_duration, int rb_effective_data_size)
    : total_allocated_rb_count(0), tti_duration(tti_duration),
      resource_block_effective_data_size(rb_effective_data_size),
      queue_count(queue_count), candidate_queue_count(0),
      fairness_of_rb_allocation_for_queues(0), _is_valid_fairness_for_queues(false),
      user_count(user_count), candidate_user_count(0),
      fairness_of_rb_allocation_for_users(0), _is_valid_fairness_for_users(false),
      throughput_for_scheduler(0), _is_valid_throughput_for_scheduler(false)
{
    initialize_queue_stats();
    initialize_user_stats();
}

void TTIStats::initialize_user_stats()
{
    for (size_t user_id = 1; user_id <= user_count; ++user_id)
    {
        user_statuses[user_id] = UserStatus();
    }
}

void TTIStats::initialize_queue_stats()
{
    for (size_t queue_id = 0; queue_id < queue_count; ++queue_id)
    {
        queue_statuses[queue_id] = QueueStatus();
    }
}

void TTIStats::mark_user_as_resource_candidate(User *user)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        user_statuses[user_id].is_resource_candidate = true;
    }
}

void TTIStats::mark_queue_as_resource_candidate(size_t queue_id)
{
    queue_statuses[queue_id].is_resource_candidate = true;
}

void TTIStats::add_allocated_rb_to_user(User *user, int rb_count)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        user_statuses[user_id].allocated_rb_count += rb_count;
    }
}

void TTIStats::add_allocated_rb_to_queue(size_t queue_id, int rb_count)
{
    queue_statuses[queue_id].allocated_rb_count += rb_count;
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
        for (auto &queue : queue_statuses)
        {
            if (queue.second.is_resource_candidate)
            {
                candidate_queue_count += 1;

                int squared_allocated_rb_count_by_queue =
                    std::pow(queue.second.allocated_rb_count, 2);
                sum_of_squared_allocated_rb_count_by_queue +=
                    squared_allocated_rb_count_by_queue;
            }
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_rb_allocation_for_queues =
            (double)squared_sum_of_allocated_rb_count /
            (double)(candidate_queue_count * sum_of_squared_allocated_rb_count_by_queue);

        _is_valid_fairness_for_queues = true;
    }
}

/*
Справедливость относительно пользователей

Общая формула: JFI = ((sum(RB_i))^2)/(N * sum(RB_i^2))
RB_i - кол-во RB выделенных пользователю i,
N - кол-во пользователей
*/

void TTIStats::calculate_fairness_for_users()
{
    _is_valid_fairness_for_users = false;

    if (total_allocated_rb_count > 0)
    {
        // sum(RB_i))^2
        int squared_sum_of_allocated_rb_count = std::pow(total_allocated_rb_count, 2);

        // sum(RB_i^2)
        int sum_of_squared_allocated_rb_count_by_user = 0;
        for (auto &user : user_statuses)
        {
            if (user.second.is_resource_candidate)
            {
                candidate_user_count += 1;

                int squared_allocated_rb_count_by_user =
                    std::pow(user.second.allocated_rb_count, 2);
                sum_of_squared_allocated_rb_count_by_user +=
                    squared_allocated_rb_count_by_user;
            }
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_rb_allocation_for_users =
            (double)squared_sum_of_allocated_rb_count /
            (double)(candidate_user_count * sum_of_squared_allocated_rb_count_by_user);

        _is_valid_fairness_for_users = true;
    }
}

double TTIStats::get_fairness_for_queues()
{
    return fairness_of_rb_allocation_for_queues;
}

double TTIStats::get_fairness_for_users()
{
    return fairness_of_rb_allocation_for_users;
}

bool TTIStats::is_valid_fairness_for_queues()
{
    return _is_valid_fairness_for_queues;
}

bool TTIStats::is_valid_fairness_for_users()
{
    return _is_valid_fairness_for_users;
}

/*
Пропускная способность планировщика (байт/мс)

Общая формула: THROUGHPUT = (sum(RB_i) * S_RB)/(T_tti)
RB_i - кол-во RB выделенных пользователю i,
S_RB - размер полезных данных (байт) в одном RB,
T_tti - интервал времени TTI (секунды)
*/

void TTIStats::calculate_throughput_for_scheduler()
{
    _is_valid_throughput_for_scheduler = false;

    if (total_allocated_rb_count > 0)
    {
        /* 
        Пропускная способность (байт/мс) 
        */
        throughput_for_scheduler =
            (total_allocated_rb_count * resource_block_effective_data_size) 
            / (tti_duration * 1000);


        _is_valid_throughput_for_scheduler = true;
    }
}

double TTIStats::get_throughput_for_scheduler()
{
    return throughput_for_scheduler;
}

bool TTIStats::is_valid_throughput_for_scheduler()
{
    return _is_valid_throughput_for_scheduler;
}