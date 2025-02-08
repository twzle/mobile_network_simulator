#include "stats/iteration_stats.hpp"

double IterationStats::get_queue_total_time(size_t queue_id)
{
    return queue_total_time[queue_id];
}

void IterationStats::set_queue_total_time(
    size_t queue_id, double total_time)
{
    queue_total_time[queue_id] = total_time;
}

double IterationStats::get_queue_processing_time(size_t queue_id)
{
    return queue_processing_time[queue_id];
}

void IterationStats::set_queue_processing_time(
    size_t queue_id, double processing_time)
{
    queue_processing_time[queue_id] = processing_time;
}

void IterationStats::increment_queue_processing_time(
    size_t queue_id, double tti_duration)
{
    set_queue_processing_time(
        queue_id,
        get_queue_processing_time(queue_id) + tti_duration);
}

void IterationStats::increment_scheduler_processing_time(
    double tti_duration)
{
    scheduler_processing_time += tti_duration;
}

double IterationStats::get_queue_idle_time(size_t queue_id)
{
    return queue_idle_time[queue_id];
}

void IterationStats::set_queue_idle_time(
    size_t queue_id, double idle_time)
{
    queue_idle_time[queue_id] = idle_time;
}

void IterationStats::increment_queue_idle_time(
    size_t queue_id, double tti_duration)
{
    set_queue_idle_time(
        queue_id,
        get_queue_idle_time(queue_id) + tti_duration);
}

void IterationStats::increment_scheduler_idle_time(
    double tti_duration)
{
    scheduler_idle_time += tti_duration;
}

double IterationStats::get_queue_wait_time(size_t queue_id)
{
    return queue_wait_time[queue_id];
}

void IterationStats::set_queue_wait_time(
    size_t queue_id, double wait_time)
{
    queue_wait_time[queue_id] = wait_time;
}

void IterationStats::increment_queue_wait_time(
    size_t queue_id, double tti_duration)
{
    set_queue_wait_time(
        queue_id,
        get_queue_wait_time(queue_id) + tti_duration);
}

void IterationStats::increment_scheduler_wait_time(
    double tti_duration)
{
    scheduler_wait_time += tti_duration;
}

void IterationStats::update_queue_time_stats(
    PacketQueueState queue_state,
    size_t queue_id,
    double tti_duration)
{
    if (queue_state == PacketQueueState::PROCESSING)
    {
        increment_queue_processing_time(
            queue_id, tti_duration);
    }
    else if (queue_state == PacketQueueState::IDLE)
    {
        increment_queue_idle_time(
            queue_id, tti_duration);
    }
    else if (queue_state == PacketQueueState::WAIT)
    {
        increment_queue_wait_time(
            queue_id, tti_duration);
    }
}

void IterationStats::update_scheduler_time_stats(
    SchedulerState scheduler_state,
    double tti_duration)
{
    if (scheduler_state == SchedulerState::PROCESSING)
    {
        increment_scheduler_processing_time(tti_duration);
    }
    else if (scheduler_state == SchedulerState::IDLE)
    {
        increment_scheduler_idle_time(tti_duration);
    }
    else if (scheduler_state == SchedulerState::WAIT)
    {
        increment_scheduler_wait_time(tti_duration);
    }
}

void IterationStats::update_scheduler_fairness_for_queues(
    double fairness_for_queues,
    bool is_valid)
{
    if (is_valid)
    {
        scheduler_fairness_for_queues.emplace_back(fairness_for_queues);
    }
}

void IterationStats::update_scheduler_fairness_for_users(
    double fairness_for_users,
    bool is_valid)
{
    if (is_valid)
    {
        scheduler_fairness_for_users.emplace_back(fairness_for_users);
    }
}

void IterationStats::update_scheduler_throughput(
    double throughput,
    bool is_valid)
{
    if (is_valid)
    {
        scheduler_throughput.emplace_back(throughput);
    }
}

void IterationStats::add_queue_packet_stats(
    size_t queue_id,
    double scheduled_at,
    double processed_at)
{
    queue_stats.emplace_back(
        queue_id,
        scheduled_at,
        processed_at - scheduled_at);
}

// Вывод статистики в stdout
void IterationStats::print()
{
    std::cout << "\nPacket count = "
              << packet_count << "\n"
              << "Total scheduling time (total_time) = "
              << scheduler_total_time << " s\n" // Общее время работы
              << "Total processing time (processing_time) = "
              << scheduler_processing_time
              << " s (" // Общее время обслуживания пакетов и доля от общего времени
              << 100 * (scheduler_processing_time / scheduler_total_time)
              << "% of all)\n"
              << "Total idle time (idle_time) = "
              << scheduler_idle_time
              << " s (" // Общее время простоя и доля от общего времени
              << 100 * (scheduler_idle_time / scheduler_total_time)
              << "% of all)\n"
              << "Total wait time (wait_time) = "
              << scheduler_wait_time
              << " s (" // Общее время простоя и доля от общего времени
              << 100 * (scheduler_wait_time / scheduler_total_time)
              << "% of all)\n"
              << "Average packet processing time ((processing_time + idle_time) / packet_count) = "
              << ((scheduler_processing_time + scheduler_idle_time) / packet_count) * 1000
              << " ms\n" // Среднее время обслуживания пакета
              << "Average packet processing delay = "
              << scheduler_average_delay * 1000
              << " ms\n" // Среднее время задержки обслуживания пакета
              << "Average fairness for queues = "
              << scheduler_average_fairness_for_queues << "\n"
              << "Average fairness for users = "
              << scheduler_average_fairness_for_users << "\n"
              << "Average scheduler throughput = " 
              << scheduler_average_throughput << " bytes/ms, " 
              << scheduler_average_throughput * 1000 / 1024 << " Kbytes/s\n\n";
}

void IterationStats::evaluate()
{
    evaluate_queue_wait_time_stats();
    evaluate_queue_idle_time_stats();
    evaluate_queue_processing_time_stats();
    evaluate_queue_total_time_stats();

    evaluate_delay_stats();
    evaluate_fairness_for_queues_stats();
    evaluate_fairness_for_users_stats();

    evaluate_throughput_stats();
}

void IterationStats::evaluate_queue_total_time_stats()
{
    double sum_of_all_queue_total_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_total_time)
    {
        sum_of_all_queue_total_time += stats.second;
    }

    queue_average_total_time =
        sum_of_all_queue_total_time / queue_total_time.size();
}

void IterationStats::evaluate_queue_processing_time_stats()
{
    double sum_of_all_queue_prcoessing_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_processing_time)
    {
        sum_of_all_queue_prcoessing_time += stats.second;
    }

    queue_average_processing_time =
        sum_of_all_queue_prcoessing_time / queue_processing_time.size();
}

void IterationStats::evaluate_queue_idle_time_stats()
{
    double sum_of_all_queue_idle_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_idle_time)
    {
        sum_of_all_queue_idle_time += stats.second;
    }

    queue_average_idle_time =
        sum_of_all_queue_idle_time / queue_idle_time.size();
}

void IterationStats::evaluate_queue_wait_time_stats()
{
    double sum_of_all_queue_wait_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_wait_time)
    {
        sum_of_all_queue_wait_time += stats.second;
    }

    queue_average_wait_time =
        sum_of_all_queue_wait_time / queue_wait_time.size();
}

void IterationStats::evaluate_fairness_for_queues_stats()
{
    double sum_of_all_fairness_for_queues = 0;
    // Подсчет суммы справедливостей за все время работы планировщика
    for (auto &stats : scheduler_fairness_for_queues)
    {
        sum_of_all_fairness_for_queues += stats;
    }

    scheduler_average_fairness_for_queues =
        sum_of_all_fairness_for_queues / scheduler_fairness_for_queues.size();
}

void IterationStats::evaluate_fairness_for_users_stats()
{
    double sum_of_all_fairness_for_users = 0;
    // Подсчет суммы справедливостей за все время работы планировщика
    for (auto &stats : scheduler_fairness_for_users)
    {
        sum_of_all_fairness_for_users += stats;
    }

    scheduler_average_fairness_for_users =
        sum_of_all_fairness_for_users / scheduler_fairness_for_users.size();
}

void IterationStats::evaluate_throughput_stats()
{
    double sum_of_all_throughputs = 0;
    // Подсчет суммы справедливостей за все время работы планировщика
    for (auto &stats : scheduler_throughput)
    {
        sum_of_all_throughputs += stats;
    }

    scheduler_average_throughput =
        sum_of_all_throughputs / scheduler_throughput.size();
}

void IterationStats::evaluate_delay_stats()
{
    std::map<int, double> total_processing_delay_by_queue;
    std::map<int, double> total_packets_by_queue;
    double total_average_delay_by_scheduler = 0;
    // Подсчет времени задержек обслуживания пакетов по очередям
    for (auto &stats : queue_stats)
    {
        total_processing_delay_by_queue[stats.queue_id] += stats.processing_delay;
        total_packets_by_queue[stats.queue_id] += 1;
    }

    for (auto &stats : queue_stats)
    {
        double total_delay = total_processing_delay_by_queue[stats.queue_id];
        size_t packet_count = total_packets_by_queue[stats.queue_id];
        this->queue_average_delay[stats.queue_id] = total_delay / packet_count;

        total_average_delay_by_scheduler += this->queue_average_delay[stats.queue_id];
    }

    size_t queue_count = total_processing_delay_by_queue.size();
    scheduler_average_delay = total_average_delay_by_scheduler / queue_count;
}

void IterationStats::release_memory_resources()
{
    queue_stats.clear();
}
