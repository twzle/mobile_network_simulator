#include "execution_stats.hpp"
#include "time_generator.hpp"
#include <iostream>

// Вывод статистики в stdout
void ExecutionStats::print()
{
    // Общее время простоя
    double total_idle_time = total_time - total_processing_time;

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
    evaluate_queue_processing_time_stats();
    evaluate_delay_stats();
}

// Рисование графика задержек
void ExecutionStats::evaluate_queue_processing_time_stats()
{
    double sum_of_all_queue_prcoessing_time = 0;
    // Подсчет общего времени работы всех очередей
    for (auto &stats : queue_processing_time)
    {
        sum_of_all_queue_prcoessing_time += stats.second;
    }

    average_queue_processing_time = 
        sum_of_all_queue_prcoessing_time / queue_processing_time.size();
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
        average_delay += average_delay_in_queue;
        this->average_delay_by_queue.push_back(average_delay_in_queue);
    }

    average_delay /= queue_stats.size();
}
