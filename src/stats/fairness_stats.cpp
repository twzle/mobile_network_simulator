#include "stats/fairness_stats.hpp"

FairnessStats::FairnessStats() {}

FairnessStats::FairnessStats(
    size_t queue_count, size_t user_count,
    double tti_duration)
    : history_size_limit(1), current_history_size(0),
      total_allocated_effective_data_size(0), tti_duration(tti_duration),
      queue_count(queue_count), candidate_queue_count(0),
      fairness_of_effective_data_allocation_for_queues(0),
      _is_valid_fairness_for_queues(false),
      user_count(user_count), candidate_user_count(0),
      fairness_of_effective_data_allocation_for_users(0),
      _is_valid_fairness_for_users(false)
{
    initialize_queue_stats();
    initialize_user_stats();
}

void FairnessStats::initialize(
    size_t queue_count, size_t user_count,
    double tti_duration, int history_size_limit)
{
    this->history_size_limit = history_size_limit;

    this->queue_count = queue_count;
    this->user_count = user_count;
    this->tti_duration = tti_duration;

    reset();
}

void FairnessStats::reset()
{
    current_history_size = 0;

    total_allocated_effective_data_size = 0;
    candidate_queue_count = 0;
    candidate_user_count = 0;
    fairness_of_effective_data_allocation_for_queues = 0;
    fairness_of_effective_data_allocation_for_users = 0;
    _is_valid_fairness_for_queues = false;
    _is_valid_fairness_for_users = false;

    initialize_queue_stats();
    initialize_user_stats();

    // std::cout << "AFTER RESET USERS = " << user_statuses.size() << "\n";
    // for (auto& user: user_statuses){
    //     std::cout << "ID = " << user.first << ", DATA = "<< user.second.allocated_effective_data_size << "\n";
    // }
}

void FairnessStats::increment_current_history_size()
{
    current_history_size++;
}

bool FairnessStats::is_history_size_limit_reached()
{
    return current_history_size == history_size_limit;
}

void FairnessStats::initialize_user_stats()
{
    for (size_t user_id = 0; user_id < user_count; ++user_id)
    {
        user_statuses[user_id] = UserFairnessStatus();
    }
}

void FairnessStats::initialize_queue_stats()
{
    for (size_t queue_id = 0; queue_id < queue_count; ++queue_id)
    {
        queue_statuses[queue_id] = QueueFairnessStatus();
    }
}

void FairnessStats::mark_user_as_resource_candidate(User *user)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        user_statuses[user_id].is_resource_candidate = true;
    }
}

void FairnessStats::mark_queue_as_resource_candidate(size_t queue_id)
{
    queue_statuses[queue_id].is_resource_candidate = true;
}

void FairnessStats::add_allocated_effective_data_to_user(
    User *user, int rb_count)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        uint8_t user_cqi = user->get_cqi();
        int effective_data_size_in_bytes_per_rb =
            StandardManager::get_resource_block_effective_data_size(user_cqi);

        int effective_data_size_in_bytes = rb_count * effective_data_size_in_bytes_per_rb;

        user_statuses[user_id].allocated_effective_data_size += effective_data_size_in_bytes;
    }
}

void FairnessStats::add_allocated_effective_data_to_queue(
    User *user, size_t queue_id, int rb_count)
{
    if (user != nullptr)
    {
        uint8_t user_cqi = user->get_cqi();
        int effective_data_size_in_bytes_per_rb =
            StandardManager::get_resource_block_effective_data_size(user_cqi);

        int effective_data_size_in_bytes = rb_count * effective_data_size_in_bytes_per_rb;

        queue_statuses[queue_id].allocated_effective_data_size += effective_data_size_in_bytes;
    }
}

void FairnessStats::add_allocated_effective_data_to_total(
    User *user, int rb_count)
{
    if (user != nullptr)
    {
        uint8_t user_cqi = user->get_cqi();
        int effective_data_size_in_bytes_per_rb =
            StandardManager::get_resource_block_effective_data_size(user_cqi);

        int effective_data_size_in_bytes = rb_count * effective_data_size_in_bytes_per_rb;

        total_allocated_effective_data_size += effective_data_size_in_bytes;
    }
}

/*
Справедливость относительно очередей

Общая формула: JFI = ((sum(RB_i))^2)/(N * sum(RB_i^2))
RB_i - кол-во RB выделенных очереди i,
N - кол-во очередей
*/

void FairnessStats::calculate_fairness_for_queues()
{
    _is_valid_fairness_for_queues = false;

    if (total_allocated_effective_data_size > 0)
    {
        // sum(RB_i))^2
        long long squared_sum_of_allocated_effective_data_size =
            std::pow(total_allocated_effective_data_size, 2);

        // sum(RB_i^2)
        long long sum_of_squared_allocated_effective_data_size_by_queue = 0;
        for (auto &queue : queue_statuses)
        {
            if (queue.second.is_resource_candidate)
            {
                candidate_queue_count += 1;

                long long squared_allocated_effective_data_size_by_queue =
                    std::pow(queue.second.allocated_effective_data_size, 2);
                sum_of_squared_allocated_effective_data_size_by_queue +=
                    squared_allocated_effective_data_size_by_queue;
            }
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_effective_data_allocation_for_queues =
            (double)squared_sum_of_allocated_effective_data_size /
            (double)(candidate_queue_count *
                     sum_of_squared_allocated_effective_data_size_by_queue);

        _is_valid_fairness_for_queues = true;
    }
}

/*
Справедливость относительно пользователей

Общая формула: JFI = ((sum(RB_i))^2)/(N * sum(RB_i^2))
RB_i - кол-во RB выделенных пользователю i,
N - кол-во пользователей
*/

void FairnessStats::calculate_fairness_for_users()
{
    _is_valid_fairness_for_users = false;

    if (total_allocated_effective_data_size > 0)
    {
        // sum(RB_i))^2
        long long squared_sum_of_allocated_effective_data_size =
            std::pow(total_allocated_effective_data_size, 2);

        // sum(RB_i^2)
        long long sum_of_squared_allocated_effective_data_size_by_user = 0;
        for (auto &user : user_statuses)
        {
            if (user.second.is_resource_candidate)
            {
                candidate_user_count += 1;

                std::cout << "ID = " << user.first << ", USER DATA = " << user.second.allocated_effective_data_size << "\n";
                long long squared_allocated_effective_data_size_by_user =
                    std::pow(user.second.allocated_effective_data_size, 2);
                sum_of_squared_allocated_effective_data_size_by_user +=
                    squared_allocated_effective_data_size_by_user;
            }
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_effective_data_allocation_for_users =
            (double)squared_sum_of_allocated_effective_data_size /
            (double)(candidate_user_count *
                     sum_of_squared_allocated_effective_data_size_by_user);

        std::cout << "JFI = " << squared_sum_of_allocated_effective_data_size << " / " << candidate_user_count << " * " << sum_of_squared_allocated_effective_data_size_by_user << "\n";

        _is_valid_fairness_for_users = true;
    }
}

double FairnessStats::get_fairness_for_queues()
{
    return fairness_of_effective_data_allocation_for_queues;
}

double FairnessStats::get_fairness_for_users()
{
    return fairness_of_effective_data_allocation_for_users;
}

bool FairnessStats::is_valid_fairness_for_queues()
{
    return _is_valid_fairness_for_queues;
}

bool FairnessStats::is_valid_fairness_for_users()
{
    return _is_valid_fairness_for_users;
}