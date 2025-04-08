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
    int tti_count,
    double fairness_for_queues,
    bool is_valid)
{
    if (is_valid)
    {
        scheduler_fairness_for_queues.emplace_back(
            std::make_pair(tti_count, fairness_for_queues)
        );
    }
}

void IterationStats::update_scheduler_fairness_for_users(
    int tti_count,
    double fairness_for_users,
    bool is_valid)
{
    if (is_valid)
    {
        std::cout << "UPDATE FAIRNESS, HISTORY SIZE = " << tti_count << "\n";
        scheduler_fairness_for_users.emplace_back(
            std::make_pair(tti_count, fairness_for_users)
        );
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
    int user_id,
    double processing_delay)
{
    packet_stats.emplace_back(
        queue_id,
        user_id,
        processing_delay);
}

// Вывод статистики в stdout
void IterationStats::print()
{
    std::cout << "\nITERATION STATS\n";
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
              << "Average fairness for queues = "
              << scheduler_average_fairness_for_queues << "\n"
              << "Average fairness for users = "
              << scheduler_average_fairness_for_users << "\n"
              << "Average scheduler throughput = "
              << scheduler_average_throughput * 1000 << " Mbytes/s, "
              << scheduler_average_throughput * 1024 << " Kbytes/ms\n"
              << "Average scheduler packet processing delay = "
              << scheduler_average_packet_processing_delay * 1000
              << " ms\n"; // Среднее время задержки обслуживания пакета

    print_queue_delays();
    print_user_delays();
}

void IterationStats::print_queue_delays(){
    for (size_t queue_id = 0;
        queue_id < queue_average_packet_processing_delay.size();
        ++queue_id)
   {
       std::cout << "Average queue packet processing delay time "
                 << "(Queue #" << queue_id << ") = "
                 << queue_average_packet_processing_delay[queue_id] * 1000 << " ms\n";
   }
}

void IterationStats::print_user_delays(){
    for (size_t user_id = 0;
        user_id < user_average_packet_processing_delay.size();
        ++user_id)
   {
       std::cout << "Average user packet processing delay time "
                 << "(User #" << user_id << ") = "
                 << user_average_packet_processing_delay[user_id] * 1000 << " ms\n";
   }
}

void IterationStats::evaluate(int queue_count, int user_count)
{
    evaluate_queue_wait_time_stats();
    evaluate_queue_idle_time_stats();
    evaluate_queue_processing_time_stats();
    evaluate_queue_total_time_stats();

    evaluate_user_delay_stats(user_count);
    evaluate_queue_delay_stats(queue_count);
    evaluate_scheduler_delay_stats();

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
    int sum_of_all_repetitions = 0;

    // Подсчет суммы справедливостей за все время работы планировщика
    for (auto &stats : scheduler_fairness_for_queues)
    {
        sum_of_all_fairness_for_queues += stats.first * stats.second;
        sum_of_all_repetitions += stats.first;
    }

    scheduler_average_fairness_for_queues =
        sum_of_all_fairness_for_queues / sum_of_all_repetitions;
}

void IterationStats::evaluate_fairness_for_users_stats()
{
    double sum_of_all_fairness_for_users = 0;
    int sum_of_all_repetitions = 0;

    std::cout << "SIZE = " << scheduler_fairness_for_users.size() << "\n";
    std::cout << "REP = " << scheduler_fairness_for_users[scheduler_fairness_for_users.size() - 1].first << ", EL =" <<
    scheduler_fairness_for_users[scheduler_fairness_for_users.size() - 1].second << "\n";
    // Подсчет суммы справедливостей за все время работы планировщика
    for (auto &stats : scheduler_fairness_for_users)
    {
        sum_of_all_fairness_for_users += stats.first * stats.second;
        sum_of_all_repetitions += stats.first;
    }

    std::cout << "REP = " << sum_of_all_repetitions << ", SUM FAIRNESS = " << sum_of_all_fairness_for_users << "\n";

    scheduler_average_fairness_for_users =
        sum_of_all_fairness_for_users / sum_of_all_repetitions;
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

void IterationStats::evaluate_queue_delay_stats(int queue_count)
{
    // Статистика по очередям
    std::map<int, double> total_queue_packet_processing_delay;
    std::map<int, double> total_queue_processed_packets;

    // Иницилизация
    for (int queue_id = 0; queue_id < queue_count; queue_id++)
    {
        total_queue_packet_processing_delay[queue_id] = 0;
        total_queue_processed_packets[queue_id] = 0;
    }

    // Подсчет времени задержек обслуживания пакетов по очередям
    for (auto &stats : packet_stats)
    {
        total_queue_packet_processing_delay[stats.queue_id] +=
            stats.processing_delay;
        total_queue_processed_packets[stats.queue_id] += 1;
    }

    for (auto &queue_delay_stats : total_queue_packet_processing_delay)
    {
        double total_packet_processing_delay_in_queue =
            total_queue_packet_processing_delay[queue_delay_stats.first];
        size_t total_packet_count_in_queue =
            total_queue_processed_packets[queue_delay_stats.first];

        if (total_packet_count_in_queue == 0)
        {
            this->queue_average_packet_processing_delay[queue_delay_stats.first] = 0;
        }
        else
        {
            this->queue_average_packet_processing_delay[queue_delay_stats.first] =
                total_packet_processing_delay_in_queue / total_packet_count_in_queue;
        }
    }
}

void IterationStats::evaluate_user_delay_stats(int user_count)
{
    // Статистика по пользователям
    std::map<int, double> total_user_packet_processing_delay;
    std::map<int, double> total_user_processed_packets;

    // Иницилизация
    for (int user_id = 0; user_id < user_count; user_id++)
    {
        total_user_packet_processing_delay[user_id] = 0;
        total_user_processed_packets[user_id] = 0;
    }

    // Подсчет времени задержек обслуживания пакетов по пользователям
    for (auto &stats : packet_stats)
    {
        total_user_packet_processing_delay[stats.user_id] +=
            stats.processing_delay;
        total_user_processed_packets[stats.user_id] += 1;
    }

    for (auto &user_delay_stats : total_user_packet_processing_delay)
    {
        double total_packet_processing_delay_for_user =
            total_user_packet_processing_delay[user_delay_stats.first];
        size_t total_packet_count_for_user =
            total_user_processed_packets[user_delay_stats.first];

        if (total_packet_count_for_user == 0)
        {
            this->user_average_packet_processing_delay[user_delay_stats.first] = 0;
        }
        else
        {
            this->user_average_packet_processing_delay[user_delay_stats.first] =
                total_packet_processing_delay_for_user / total_packet_count_for_user;
        }
    }
}

void IterationStats::evaluate_scheduler_delay_stats()
{   
    int queue_count = this->queue_average_packet_processing_delay.size();

    double total_average_scheduler_packet_processing_delay = 0;

    for (auto& queue_avg_delay_stats : this->queue_average_packet_processing_delay){
        total_average_scheduler_packet_processing_delay += 
            queue_avg_delay_stats.second;
    }

    this->scheduler_average_packet_processing_delay =
        total_average_scheduler_packet_processing_delay / queue_count;
}

void IterationStats::release_memory_resources()
{
    packet_stats.clear();
}
