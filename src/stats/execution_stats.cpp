#include "stats/execution_stats.hpp"
#include "core/time_generator.hpp"
#include <iostream>


// Вывод статистики в stdout
void ExecutionStats::print()
{
    std::cout << "\nTotal running time = "
              << total_time << " ms\n" // Общее время работы
              << "Total processing time = "
              << total_processing_time 
              << " ms (" // Общее время обслуживания пакетов и доля от общего времени
              << 100 * (total_processing_time / total_time) 
              << "% of all)\n"
              << "Total idle time = "
              << total_idle_time 
              << " ms (" // Общее время простоя и доля от общего времени
              << 100 * (total_idle_time / total_time) 
              << "% of all)\n"
              << "Average packet processing time = "
              << total_processing_time / packet_count 
              << " ms\n" // Среднее время обслуживания пакета
              << "Average packet processing delay = "
              << average_delay_by_scheduler 
              << " ms\n" // Среднее время задержки обслуживания пакета
              << "Retried packet count = " // Количество пакетов обслуженных не с первого раза и их доля
              << retried_packet_count 
              << " (" << ((double)retried_packet_count / packet_count * 100) 
              << "% of all)\n"
              << "Packet loss = " // Количество пакетов обслуженных не с первого раза и их доля
              << lost_packet_count << " (" 
              << ((double)lost_packet_count / (packet_count + lost_packet_count) * 100) 
              << "% of all)\n\n";
}

void ExecutionStats::evaluate(){
    evaluate_values();
    evaluate_queue_processing_time_stats();
    evaluate_delay_stats();
}

void ExecutionStats::evaluate_values(){
    total_idle_time = total_time - total_processing_time;
}

// Рисование графика задержек
void ExecutionStats::evaluate_queue_processing_time_stats()
{
    double sum_of_all_queue_prcoessing_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : processing_time_by_queue)
    {
        sum_of_all_queue_prcoessing_time += stats.second;
    }

    average_processing_time_by_scheduler = 
        sum_of_all_queue_prcoessing_time / processing_time_by_queue.size();
}


// Рисование графика задержек
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

void ExecutionStats::release_memory_resources(){
    queue_stats.clear();
}
