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
}

// Подсчет среднего арифметического базовых параметров работы планировщика
void AverageStats::calculate_average_values()
{
    for (auto &stats : stats_array)
    {
        common_total_time += stats.total_time;
        common_total_skip_time += stats.total_skip_time;
        common_total_processing_time += stats.total_processing_time;
        common_total_packet_count += stats.packet_count;
        common_total_lost_packet_count += stats.lost_packet_count;
        common_total_retried_packet_count += stats.retried_packet_count;
    }

    common_total_idle_time = common_total_time - common_total_processing_time;

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
    average_delays.resize(queue_count);

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            average_delays[i] += stats.average_delay_by_queue[i];
        }
        total_average_delay += stats.average_delay;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        average_delays[i] /= stats_array.size();
    }

    total_average_delay /= stats_array.size();
}

void AverageStats::calculate_average_queue_processing_time()
{
    int queue_count = stats_array[0].queue_processing_time.size();
    average_queue_processing_time.resize(queue_count);

    double total_average_processing_time = 0;

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            average_queue_processing_time[i] += stats.queue_processing_time[i];
        }
        total_average_processing_time += stats.average_queue_processing_time;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        average_queue_processing_time[i] /= stats_array.size();
    }

    total_average_queue_processing_time = total_average_processing_time / stats_array.size();
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
              << " ms\n"                   // Среднее время обслуживания пакета
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
        << YAML::Value << average_queue_processing_time;

    // total_average_queue_processing_time
    out << YAML::Key << "total_average_queue_processing_time" 
        << YAML::Value << total_average_queue_processing_time;

    // average_delays
    out << YAML::Key << "average_delays" 
        << YAML::Value << average_delays;

    // total_average_delay
    out << YAML::Key << "total_average_delay" 
        << YAML::Value << total_average_delay;

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