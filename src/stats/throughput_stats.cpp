#include "stats/throughput_stats.hpp"

ThroughputStats::ThroughputStats() {}

ThroughputStats::ThroughputStats(
    size_t queue_count, size_t user_count,
    double tti_duration)
    : history_size_limit(1), current_history_size(0),
      total_allocated_effective_data_size(0), total_allocated_resource_blocks(0),
      max_allocated_resource_blocks(0), tti_duration(tti_duration),
      queue_count(queue_count), candidate_queue_count(0),
      user_count(user_count), candidate_user_count(0),
      throughput_for_scheduler(0), _is_valid_throughput_for_scheduler(false),
      unused_resources_for_scheduler(0), _is_valid_unused_resources_for_scheduler(false)
{
    initialize_queue_stats();
    initialize_user_stats();
}

void ThroughputStats::initialize(
    int resource_block_limit_per_tti,
    size_t queue_count, size_t user_count,
    double tti_duration, int history_size_limit)
{
    this->history_size_limit = history_size_limit;

    this->queue_count = queue_count;
    this->user_count = user_count;
    this->tti_duration = tti_duration;

    this->max_allocated_resource_blocks = resource_block_limit_per_tti;

    reset();
}

void ThroughputStats::reset()
{
    current_history_size = 0;

    total_allocated_effective_data_size = 0;
    total_allocated_resource_blocks = 0;
    candidate_queue_count = 0;
    candidate_user_count = 0;

    throughput_for_scheduler = 0;
    unused_resources_for_scheduler = 0;
    _is_valid_throughput_for_scheduler = false;
    _is_valid_unused_resources_for_scheduler = false;

    initialize_queue_stats();
    initialize_user_stats();
}

int ThroughputStats::get_current_history_size()
{
    return current_history_size;
}

void ThroughputStats::increment_current_history_size()
{
    current_history_size++;
}

bool ThroughputStats::is_history_size_limit_reached()
{
    return current_history_size == history_size_limit;
}

void ThroughputStats::initialize_user_stats()
{
    for (size_t user_id = 0; user_id < user_count; ++user_id)
    {
        user_statuses[user_id] = UserThroughputStatus();
    }
}

void ThroughputStats::initialize_queue_stats()
{
    for (size_t queue_id = 0; queue_id < queue_count; ++queue_id)
    {
        queue_statuses[queue_id] = QueueThroughputStatus();
    }
}

void ThroughputStats::mark_user_as_resource_candidate(User *user)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        user_statuses[user_id].is_resource_candidate = true;
    }
}

void ThroughputStats::mark_queue_as_resource_candidate(size_t queue_id)
{
    queue_statuses[queue_id].is_resource_candidate = true;
}

void ThroughputStats::add_allocated_effective_data_to_user(
    User *user, int packet_size_in_bytes)
{
    if (user != nullptr)
    {
        int user_id = user->get_id();
        user_statuses[user_id].allocated_effective_data_size += packet_size_in_bytes;
    }
}

void ThroughputStats::add_allocated_effective_data_to_queue(
    size_t queue_id, int packet_size_in_bytes)
{
    queue_statuses[queue_id].allocated_effective_data_size += packet_size_in_bytes;
}

void ThroughputStats::add_allocated_effective_data_to_total(
    int packet_size_in_bytes, int resource_blocks)
{
    total_allocated_effective_data_size += packet_size_in_bytes;
    total_allocated_resource_blocks += resource_blocks;
}

/*
Пропускная способность планировщика (байт/мс)

Общая формула: THROUGHPUT = (sum(RB_i) * S_RB)/(T_tti)
RB_i - кол-во RB выделенных пользователю i,
S_RB - размер полезных данных (байт) в одном RB,
T_tti - интервал времени TTI (секунды)
*/

void ThroughputStats::calculate_throughput_for_scheduler()
{
    _is_valid_throughput_for_scheduler = false;

    // Если было передано ненулевое число байт, значит нужно учесть throughput
    if (total_allocated_effective_data_size > 0)
    {
        /*
        Пропускная способность (Мбит/мс) =
        (Размер данных выделенных за TTI (байт/мс) * 8 / (1000 * 1000) / )
        */

        // Пропускная способность (байт/мс) -> (бит/мс)
        double bits_per_ms = total_allocated_effective_data_size * 8;

        // Пропускная способность (Мбит/мс)
        throughput_for_scheduler = bits_per_ms / (1000 * 1000);

        _is_valid_throughput_for_scheduler = true;

        return;
    }

    // Если было передано нулевое число байт, значит перед учетом throughput
    // нужно проверить были ли кандидаты на ресурсы
    if (total_allocated_effective_data_size == 0)
    {
        // Подсчет очередей имевших трафик в этом TTI
        int candidate_queue_count = 0;
        for (auto &queue : queue_statuses)
        {
            if (queue.second.is_resource_candidate)
            {
                ++candidate_queue_count;
            }
        }

        // Если были участники распределения (очереди), имевшие трафик
        if (candidate_queue_count > 0)
        {
            /*
            Пропускная способность (Мбайт/мс) =
            (Размер данных выделенных за TTI (байт/мс) / 1024 * 1024)
            */
            throughput_for_scheduler = 0;

            _is_valid_throughput_for_scheduler = true;
        }
    }
}

double ThroughputStats::get_throughput_for_scheduler()
{
    return throughput_for_scheduler;
}

bool ThroughputStats::is_valid_throughput_for_scheduler()
{
    return _is_valid_throughput_for_scheduler;
}

void ThroughputStats::calculate_unused_resources_for_scheduler()
{
    _is_valid_unused_resources_for_scheduler = false;

    // Если было передано ненулевое число байт, значит нужно учесть throughput
    if (total_allocated_resource_blocks > 0)
    {
        /*
        Доля неиспользованных ресурсов = 1 - (RB/RB_max) 
        */

        double used_resources_for_scheduler = 
            (double) total_allocated_resource_blocks / 
            (double) max_allocated_resource_blocks;
        
        unused_resources_for_scheduler = 1 - used_resources_for_scheduler;

        _is_valid_unused_resources_for_scheduler = true;

        return;
    }

    // Если было передано нулевое число байт, значит перед учетом throughput
    // нужно проверить были ли кандидаты на ресурсы
    if (total_allocated_resource_blocks == 0)
    {
        // Подсчет очередей имевших трафик в этом TTI
        int candidate_queue_count = 0;
        for (auto &queue : queue_statuses)
        {
            if (queue.second.is_resource_candidate)
            {
                ++candidate_queue_count;
            }
        }

        // Если были участники распределения (очереди), имевшие трафик
        if (candidate_queue_count > 0)
        {
            /*
            Доля неиспользованных ресурсов =
            Максимальное число ресурсных блоков за TTI
            */
            unused_resources_for_scheduler = 1;

            _is_valid_unused_resources_for_scheduler = true;

            return;
        }
    }
}

double ThroughputStats::get_unused_resources_for_scheduler()
{
    return unused_resources_for_scheduler;
}

bool ThroughputStats::is_valid_unused_resources_for_scheduler()
{
    return _is_valid_unused_resources_for_scheduler;
}

