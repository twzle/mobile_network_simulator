#include "stats/tti_stats.hpp"

TTIStats::TTIStats(size_t queue_count, size_t user_count)
    : total_allocated_rb_count(0),
      queue_count(queue_count),
      fairness_of_rb_allocation_for_queues(0), _is_valid_fairness_for_queues(false),
      user_count(user_count),
      fairness_of_rb_allocation_for_users(0), _is_valid_fairness_for_users(false)
{
    initialize_queue_stats();
    initialize_user_stats();
}

void TTIStats::initialize_user_stats()
{
    for (size_t user_id = 1; user_id <= user_count; ++user_id)
    {
        allocated_rb_count_per_user[user_id] = 0;
    }
}

void TTIStats::initialize_queue_stats()
{
    for (size_t queue_id = 0; queue_id < queue_count; ++queue_id)
    {
        allocated_rb_count_per_queue[queue_id] = 0;
    }
}

void TTIStats::add_allocated_rb_to_user(User *user, int rb_count)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        allocated_rb_count_per_user[user_id] += rb_count;
    }
}

void TTIStats::add_allocated_rb_to_queue(size_t queue_id, int rb_count)
{
    allocated_rb_count_per_queue[queue_id] += rb_count;
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
            int squared_allocated_rb_count_by_queue = std::pow(queue.second, 2);
            sum_of_squared_allocated_rb_count_by_queue +=
                squared_allocated_rb_count_by_queue;
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_rb_allocation_for_queues =
            (double) squared_sum_of_allocated_rb_count /
            (double) (queue_count * sum_of_squared_allocated_rb_count_by_queue);

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
        for (auto &user : allocated_rb_count_per_user)
        {
            int squared_allocated_rb_count_by_user = std::pow(user.second, 2);
            sum_of_squared_allocated_rb_count_by_user +=
                squared_allocated_rb_count_by_user;
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_rb_allocation_for_users =
            (double) squared_sum_of_allocated_rb_count /
            (double) (user_count * sum_of_squared_allocated_rb_count_by_user);

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

void TTIStats::add_allocated_rb_to_total(int rb_count)
{
    total_allocated_rb_count += rb_count;
}