#include "scheduling/base_drr_scheduler.hpp"

BaseDRRScheduler::BaseDRRScheduler(double tti)
    : tti_duration(tti) {}

/*
Планирование очереди через запись в массив очередей
и вычисление новой суммы общего количества пакетов
*/
void BaseDRRScheduler::schedule(PacketQueue &&packet_queue)
{
    scheduled_queues.push_back(packet_queue);
    total_packets += packet_queue.size();
}

void BaseDRRScheduler::set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit)
{
    this->resource_blocks_per_tti = resource_blocks_per_tti_limit;
}

void BaseDRRScheduler::set_initial_queue(size_t new_initial_queue_id)
{
    this->current_initial_absolute_queue_id = new_initial_queue_id;
}

size_t BaseDRRScheduler::get_initial_queue()
{
    return this->current_initial_absolute_queue_id;
}

size_t BaseDRRScheduler::get_relative_queue_id(size_t current_absolute_queue_id)
{
    size_t bias = this->current_initial_absolute_queue_id;
    size_t current_relative_queue_id = current_absolute_queue_id + bias;

    if (current_relative_queue_id >= scheduled_queues.size())
    {
        return current_relative_queue_id % scheduled_queues.size();
    }
    else
    {
        return current_relative_queue_id;
    }
}

void BaseDRRScheduler::increment_processed_packet_count(int increment)
{
    this->processed_packets += increment;
}

void BaseDRRScheduler::set_scheduling_start_time(double time)
{
    this->scheduling_start = time;
}

void BaseDRRScheduler::set_scheduling_end_time(double time)
{
    this->scheduling_end = time;
}

double BaseDRRScheduler::get_scheduling_start_time()
{
    return this->scheduling_start;
}

double BaseDRRScheduler::get_scheduling_end_time()
{
    return this->scheduling_end;
}

/*
Подсчет статистики по результатам работы планировщика
*/
void BaseDRRScheduler::evaluate_stats()
{
    stats.scheduler_total_time = scheduling_end - scheduling_start;
    stats.packet_count = total_packets;

    for (auto &queue : scheduled_queues)
    {
        stats.lost_packet_count += queue.get_lost_packet_count();
    }
}

ExecutionStats &BaseDRRScheduler::get_stats()
{
    return this->stats;
}