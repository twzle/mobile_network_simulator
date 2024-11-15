#include <iostream>
#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "average_stats.hpp"

#define DATA_DIR "data/"
#define FILE_NAME "data.yaml"

// Подсчет средних значений
void AverageStats::calculate()
{
    calculate_average_values();
    calculate_average_delays();
    calculate_average_queue_processing_time();

    calculate_standard_deviation_for_delay();
    calculate_execution_count_for_delay();
}

// Подсчет среднего арифметического базовых параметров работы планировщика
void AverageStats::calculate_average_values()
{
    for (auto &stats : stats_array)
    {
        common_total_time += stats.total_time;
        common_total_skip_time += stats.total_skip_time;
        common_total_idle_time += stats.total_idle_time;
        common_total_processing_time += stats.total_processing_time;
        common_total_packet_count += stats.packet_count;
        common_total_lost_packet_count += stats.lost_packet_count;
        common_total_retried_packet_count += stats.retried_packet_count;
    }

    average_total_time =
        common_total_time / stats_array.size();
    average_total_idle_time =
        common_total_idle_time / stats_array.size();
    average_total_skip_time =
        common_total_skip_time / stats_array.size();
    average_total_processing_time =
        common_total_processing_time / stats_array.size();
    average_total_packet_count =
        common_total_packet_count / stats_array.size();
    average_total_lost_packet_count =
        common_total_lost_packet_count / stats_array.size();
    average_total_retried_packet_count =
        common_total_retried_packet_count / stats_array.size();
}

// Подсчет среднего арифметического задержек обработки пакетов
void AverageStats::calculate_average_delays()
{
    int queue_count = stats_array[0].average_delay_by_queue.size();
    average_delays_by_queue.resize(queue_count);

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            average_delays_by_queue[i] += stats.average_delay_by_queue[i];
        }
        total_average_delay_by_scheduler += stats.average_delay_by_scheduler;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        average_delays_by_queue[i] /= stats_array.size();
    }

    total_average_delay_by_scheduler /= stats_array.size();
}

void AverageStats::calculate_average_queue_processing_time()
{
    int queue_count = stats_array[0].processing_time_by_queue.size();
    average_processing_time_by_queue.resize(queue_count);

    double total_average_processing_time = 0;

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            average_processing_time_by_queue[i] += stats.processing_time_by_queue[i];
        }
        total_average_processing_time += stats.average_processing_time_by_scheduler;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        average_processing_time_by_queue[i] /= stats_array.size();
    }

    total_average_processing_time_by_scheduler 
        = total_average_processing_time / stats_array.size();
}

void AverageStats::calculate_standard_deviation_for_delay(){
    // sum(Xi - X)^2
    double total_deviation = 0;
    for (auto &stats : stats_array){
        double difference 
            = stats.average_delay_by_scheduler - total_average_delay_by_scheduler;
        total_deviation += difference * difference;
    }

    // sum(Xi - X)^2 / (n - 1)
    double square_of_deviation 
        = total_deviation/(stats_array.size() - 1);
    
    // sqrt(sum(Xi - X)^2 / (n - 1))
    standard_devaition_delay_by_scheduler = std::sqrt(square_of_deviation);
    std::cout << "Стандартное отклонение средней задержки обслуживания = " 
        << standard_devaition_delay_by_scheduler << " ms\n"; 
}

void AverageStats::calculate_execution_count_for_delay(){
    double credability_out_of_95 = 1.96; // z
    double credability_out_of_99 = 2.576; // z
    double accuracy_of_deviation = 1; // E (ms)
    
    // (z * S / E)^2
    double product_for_95 
        = (credability_out_of_95 * standard_devaition_delay_by_scheduler / accuracy_of_deviation);
    
    double product_for_99 
        = (credability_out_of_99 * standard_devaition_delay_by_scheduler / accuracy_of_deviation);
    
    double count_for_95 = std::pow(product_for_95, 2);
    double count_for_99 = std::pow(product_for_99, 2); 

    std::cout << "E (допустимая погрешность) = " << accuracy_of_deviation << " ms\n";
    std::cout << "Количество запусков для достоверности 95% = " << count_for_95 << "\n"; 
    std::cout << "Количество запусков для достоверности 99% = " << count_for_99 << "\n"; 
}


// Вывод в stdout срденего арифметического по статистике работы планировщика
void AverageStats::show()
{
    std::cout << "--------------\n\n";
    std::cout << "Average stats\n";
    std::cout << "\nRunning time = "
              << average_total_time << " ms\n" // Общее время работы
              << "Processing time = "
              << average_total_processing_time
              << " ms (" // Общее время обслуживания пакетов и доля от общего времени
              << 100 * (average_total_processing_time / average_total_time)
              << "% of all)\n"
              << "Common total idle time = "
              << average_total_idle_time
              << " ms (" // Общее время простоя и доля от общего времени
              << 100 * (average_total_idle_time / average_total_time)
              << "% of all)\n"
              << "Average packet processing time = "
              << average_total_processing_time / average_total_packet_count
              << " ms\n" // Среднее время обслуживания пакета
              << "Average packet delay time = "
              << total_average_delay_by_scheduler
              << " ms\n" // Среднее время обслуживания пакета
              << "Retried packet count = " // Количество пакетов обслуженных не с первого раза и их доля
              << average_total_retried_packet_count
              << " (" << ((double)average_total_retried_packet_count / average_total_packet_count * 100)
              << "% of all)\n"
              << "Packet loss = " // Количество пакетов обслуженных не с первого раза и их доля
              << average_total_lost_packet_count << " ("
              << ((double)average_total_lost_packet_count / (average_total_packet_count + average_total_lost_packet_count) * 100)
              << "% of all)\n\n";
}

std::string AverageStats::write_yaml()
{
    YAML::Emitter out;

    out << YAML::BeginMap;

    // average_queue_processing_time
    out << YAML::Key << "average_queue_processing_time" 
        << YAML::Value << average_processing_time_by_queue;

    // total_average_queue_processing_time
    out << YAML::Key << "total_average_queue_processing_time" 
        << YAML::Value << total_average_processing_time_by_scheduler;

    // average_delays
    out << YAML::Key << "average_delays" 
        << YAML::Value << average_delays_by_queue;

    // total_average_delay
    out << YAML::Key << "total_average_delay" 
        << YAML::Value << total_average_delay_by_scheduler;

    out << YAML::EndMap;

    std::string local_file_path = std::string(DATA_DIR) + std::string(FILE_NAME);
    std::string relative_file_path = "./" + local_file_path;
    std::ofstream fout(relative_file_path);

    fout << out.c_str();
    fout.close();

    std::string absolute_file_path = 
        std::filesystem::current_path().c_str() + std::string("/") + local_file_path;
    return absolute_file_path;
}