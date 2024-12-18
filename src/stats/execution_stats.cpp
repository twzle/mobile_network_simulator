#include "stats/execution_stats.hpp"
#include "core/time_generator.hpp"
#include <iostream>

double ExecutionStats::get_queue_total_time(size_t queue_id)
{
    return queue_total_time[queue_id];
}

void ExecutionStats::set_queue_total_time(
    size_t queue_id, double total_time)
{
    queue_total_time[queue_id] = total_time;
}

double ExecutionStats::get_queue_processing_time(size_t queue_id)
{
    return queue_processing_time[queue_id];
}

void ExecutionStats::set_queue_processing_time(
    size_t queue_id, double processing_time)
{
    queue_processing_time[queue_id] = processing_time;
}

void ExecutionStats::increment_queue_processing_time(
    size_t queue_id, double tti_duration)
{
    set_queue_processing_time(
        queue_id,
        get_queue_processing_time(queue_id) + tti_duration);
}

void ExecutionStats::increment_scheduler_processing_time(
    double tti_duration)
{
    scheduler_processing_time += tti_duration;
}

double ExecutionStats::get_queue_idle_time(size_t queue_id)
{
    return queue_idle_time[queue_id];
}

void ExecutionStats::set_queue_idle_time(
    size_t queue_id, double idle_time)
{
    queue_idle_time[queue_id] = idle_time;
}

void ExecutionStats::increment_queue_idle_time(
    size_t queue_id, double tti_duration)
{
    set_queue_idle_time(
        queue_id,
        get_queue_idle_time(queue_id) + tti_duration);
}

void ExecutionStats::increment_scheduler_idle_time(
    double tti_duration)
{
    scheduler_idle_time += tti_duration;
}

void ExecutionStats::update_queue_time_stats(
    int allocated_resource_blocks, 
    size_t queue_id, 
    double tti_duration)
{
    if (allocated_resource_blocks > 0)
    {
        increment_queue_processing_time(
            queue_id, tti_duration);
    }
    else
    {
        increment_queue_idle_time(
            queue_id, tti_duration);
    }
}

void ExecutionStats::update_scheduler_time_stats(
    int allocated_resource_blocks,  
    double tti_duration)
{
    if (allocated_resource_blocks > 0)
    {
        increment_scheduler_processing_time(tti_duration);
    }
    else
    {
        increment_scheduler_idle_time(tti_duration);
    }
}

void ExecutionStats::add_queue_packet_stats(
    size_t queue_id,
    double scheduled_at,
    double processed_at)
{
    queue_stats[queue_id].emplace_back(
        scheduled_at,
        processed_at - scheduled_at);
}

// Вывод статистики в stdout
void ExecutionStats::print()
{
    std::cout << "\nTotal schdeuling time = "
              << scheduler_total_time << " ms\n" // Общее время работы
              << "Total processing time = "
              << scheduler_processing_time
              << " ms (" // Общее время обслуживания пакетов и доля от общего времени
              << 100 * (scheduler_processing_time / scheduler_total_time)
              << "% of all)\n"
              << "Total idle time = "
              << scheduler_idle_time
              << " ms (" // Общее время простоя и доля от общего времени
              << 100 * (scheduler_idle_time / scheduler_total_time)
              << "% of all)\n"
              << "Average packet processing time = "
              << scheduler_processing_time / packet_count
              << " ms\n" // Среднее время обслуживания пакета
              << "Average packet processing delay = "
              << average_delay_by_scheduler
              << " ms\n"                   // Среднее время задержки обслуживания пакета
              << "Retried packet count = " // Количество пакетов обслуженных не с первого раза и их доля
              << retried_packet_count
              << " (" << ((double)retried_packet_count / packet_count * 100)
              << "% of all)\n"
              << "Packet loss = " // Количество пакетов обслуженных не с первого раза и их доля
              << lost_packet_count << " ("
              << ((double)lost_packet_count / (packet_count + lost_packet_count) * 100)
              << "% of all)\n\n";
}

void ExecutionStats::evaluate()
{
    evaluate_values();
    evaluate_queue_total_time_stats();
    evaluate_queue_processing_time_stats();
    evaluate_queue_idle_time_stats();
    evaluate_delay_stats();
}

void ExecutionStats::evaluate_values()
{
    scheduler_idle_time = scheduler_total_time - scheduler_processing_time;
}

void ExecutionStats::evaluate_queue_total_time_stats()
{
    double sum_of_all_queue_prcoessing_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_total_time)
    {
        sum_of_all_queue_prcoessing_time += stats.second;
    }

    scheduler_average_total_time =
        sum_of_all_queue_prcoessing_time / queue_total_time.size();
}

void ExecutionStats::evaluate_queue_processing_time_stats()
{
    double sum_of_all_queue_prcoessing_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_processing_time)
    {
        sum_of_all_queue_prcoessing_time += stats.second;
    }

    scheduler_average_processing_time =
        sum_of_all_queue_prcoessing_time / queue_processing_time.size();
}

void ExecutionStats::evaluate_queue_idle_time_stats()
{
    double sum_of_all_queue_prcoessing_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_idle_time)
    {
        sum_of_all_queue_prcoessing_time += stats.second;
    }

    scheduler_average_idle_time =
        sum_of_all_queue_prcoessing_time / queue_idle_time.size();
}

void ExecutionStats::evaluate_delay_stats()
{
    // Подсчет времени задержек обслуживания пакетов по очередям
    for (auto &stats : queue_stats)
    {
        // Подсчет времени задержек обслуживания пакетов в очереди
        double total_delay = 0;
        for (auto &packet_stats : stats.second)
        {
            total_delay += packet_stats.processing_delay;
        }

        double average_delay_in_queue = total_delay / stats.second.size();
        average_delay_by_scheduler += average_delay_in_queue;
        this->average_delay_by_queue.push_back(average_delay_in_queue);
    }

    average_delay_by_scheduler /= queue_stats.size();
}

void ExecutionStats::release_memory_resources()
{
    queue_stats.clear();
}
