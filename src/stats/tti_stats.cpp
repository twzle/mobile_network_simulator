#include "stats/tti_stats.hpp"

TTIStats::TTIStats(
    std::string standard_name,
    size_t queue_count, size_t user_count,
    double tti_duration)
    : standard_name(standard_name),
      total_allocated_effective_data_size(0), tti_duration(tti_duration),
      queue_count(queue_count), candidate_queue_count(0),
      fairness_of_effective_data_allocation_for_queues(0),
      _is_valid_fairness_for_queues(false),
      user_count(user_count), candidate_user_count(0),
      fairness_of_effective_data_allocation_for_users(0),
      _is_valid_fairness_for_users(false),
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

void TTIStats::add_allocated_effective_data_to_user(
    User *user, int rb_count)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        uint8_t user_cqi = user->get_cqi();
        int effective_data_size_per_rb =
            StandardManager::get_cqi_efficiency(
                standard_name, user_cqi) *
            StandardManager::get_resource_elements_in_resource_block(
                standard_name);
        int effective_data_size = rb_count * effective_data_size_per_rb;

        user_statuses[user_id].allocated_effective_data_size += effective_data_size;
    }
}

void TTIStats::add_allocated_effective_data_to_queue(
    User *user, size_t queue_id, int rb_count)
{
    if (user != nullptr)
    {
        uint8_t user_cqi = user->get_cqi();
        double effective_data_size_per_rb =
            StandardManager::get_cqi_efficiency(
                standard_name, user_cqi) *
            StandardManager::get_resource_elements_in_resource_block(
                standard_name);
        int effective_data_size = rb_count * effective_data_size_per_rb;

        queue_statuses[queue_id].allocated_effective_data_size += effective_data_size;
    }
}

void TTIStats::add_allocated_effective_data_to_total(
    User *user, int rb_count)
{
    if (user != nullptr)
    {
        uint8_t user_cqi = user->get_cqi();
        double effective_data_size_per_rb =
            StandardManager::get_cqi_efficiency(
                standard_name, user_cqi) *
            StandardManager::get_resource_elements_in_resource_block(
                standard_name);

        int effective_data_size = rb_count * effective_data_size_per_rb;

        total_allocated_effective_data_size += effective_data_size;
    }
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
            (double) squared_sum_of_allocated_effective_data_size /
            (double) (candidate_queue_count *
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

void TTIStats::calculate_fairness_for_users()
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

                long long squared_allocated_effective_data_size_by_user =
                    std::pow(user.second.allocated_effective_data_size, 2);
                sum_of_squared_allocated_effective_data_size_by_user +=
                    squared_allocated_effective_data_size_by_user;
            }
        }

        // ((sum(RB_i))^2)/(N * sum(RB_i^2))
        fairness_of_effective_data_allocation_for_users =
            (double) squared_sum_of_allocated_effective_data_size /
            (double) (candidate_user_count *
                     sum_of_squared_allocated_effective_data_size_by_user);

        _is_valid_fairness_for_users = true;
    }
}

double TTIStats::get_fairness_for_queues()
{
    return fairness_of_effective_data_allocation_for_queues;
}

double TTIStats::get_fairness_for_users()
{
    return fairness_of_effective_data_allocation_for_users;
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

    if (total_allocated_effective_data_size > 0)
    {
        /*
        Пропускная способность (Кбайт/мс) = 
        ((Размер данных выделенных за TTI (бит) / 8) / 1024) (Кбайт)
        / 
        (Длительность TTI (секунды) * 1000) (миллисекунды)
        */
        throughput_for_scheduler =
            (((double) total_allocated_effective_data_size / (double) 8) / (double) 1024) 
            / (double)(tti_duration * 1000);

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