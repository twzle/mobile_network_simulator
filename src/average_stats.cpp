#include <matplotlibcpp.h>
#include <iostream>
#include "average_stats.hpp"

namespace plt = matplotlibcpp;

// Подсчет средних значений
void AverageStats::calculate(){
    calculate_average_values();
    calculate_average_delays();
    calculate_average_packet_scheduling_time();
}

// Подсчет среднего арифметического базовых параметров работы планировщика
void AverageStats::calculate_average_values(){
    for (auto &stats : stats_array){
        common_total_time += stats.total_time.count();
        common_total_skip_time += stats.total_skip_time.count();
        common_total_processing_time += stats.total_processing_time.count();
        common_total_packet_count += stats.packet_count;
        common_total_lost_packet_count += stats.lost_packet_count;
        common_total_retried_packet_count += stats.retried_packet_count;
    }

    common_total_idle_time = common_total_time - common_total_processing_time;

    average_total_time = 
        common_total_time / float(stats_array.size());
    average_total_idle_time = 
        common_total_idle_time / float(stats_array.size());
    average_total_skip_time = 
        common_total_skip_time / float(stats_array.size());
    average_total_processing_time = 
        common_total_processing_time / float(stats_array.size());
    average_total_packet_count =
        common_total_packet_count / float(stats_array.size());
    average_total_lost_packet_count =
        common_total_lost_packet_count / float(stats_array.size());
    average_total_retried_packet_count =
        common_total_retried_packet_count / float(stats_array.size());
}

// Подсчет среднего арифметического задержек обработки пакетов
void AverageStats::calculate_average_delays(){
    int queue_count = stats_array[0].average_delays.size();
    average_delays.resize(queue_count);

    for (auto &stats : stats_array){
        for (int i = 0; i < queue_count; ++i){
            average_delays[i] += stats.average_delays[i];
        }
    }

    for (int i = 0; i < queue_count; ++i){
        average_delays[i] /= stats_array.size();
    }
}

void AverageStats::calculate_average_packet_scheduling_time(){
    int max_timestamps_count_on_graph = 20;
    int timestamps_count = stats_array[0].packets_scheduled_by_ms[0].size();

    for (auto &scheduling_stats : stats_array[0].packets_scheduled_by_ms){
        average_packets_scheduled_by_ms[scheduling_stats.first].resize(timestamps_count);
    }

    /*
     Суммирование количества запланированных пакетов 
     в единицу времени по каждой очереди за все запуски
    */
    for (auto &stats : stats_array){
        for (auto &scheduling_stats : stats.packets_scheduled_by_ms){
            for (size_t i = 0; i < scheduling_stats.second.size(); ++i){
                average_packets_scheduled_by_ms[scheduling_stats.first].at(i) 
                    += scheduling_stats.second.at(i);
            }
        }
    }
    

    /*
     Вычисление среднего арифметического количества запланированных пакетов 
     в единицу времени по каждой очереди за все запуски
    */
    for (auto &scheduling_stats : average_packets_scheduled_by_ms){
        for (size_t i = 0; i < scheduling_stats.second.size(); ++i){
            scheduling_stats.second.at(i) /= stats_array.size();
        }
    }
    
    int window_size = max_timestamps_count_on_graph; // Размер окна для сглаживания, может быть изменен для более сильного или слабого сглаживания
    if (timestamps_count <= max_timestamps_count_on_graph) {
        return;
    } else {
        window_size = timestamps_count / max_timestamps_count_on_graph;
    }


    /*
     Применение скользящего среднего для сглаживания данных
    */
    for (auto &scheduling_stats : average_packets_scheduled_by_ms){
        std::vector<float> smoothed_data(scheduling_stats.second.size(), 0.0);
        for (size_t i = 0; i < scheduling_stats.second.size(); ++i){
            double sum = 0.0;
            int count = 0;
            // Усреднение по окну
            for (int j = -window_size / 2; j <= window_size / 2; ++j){
                int index = i + j;
                if (index >= 0 && index < int(scheduling_stats.second.size())){
                    sum += scheduling_stats.second[index];
                    ++count;
                }
            }
            smoothed_data[i] = sum / count;
        }
        // Обновляем данные на сглаженные
        scheduling_stats.second = smoothed_data;
    }
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
        << 100 * ((float)average_total_processing_time 
            / (float)average_total_time) 
        << "% of all)\n"
        << "Common total idle time = "
        << average_total_idle_time 
        << " ms (" // Общее время простоя и доля от общего времени
        << 100 * ((float)average_total_idle_time 
            / (float)average_total_time) 
        << "% of all)\n"
        << "Average packet processing time = "
        << average_total_processing_time / average_total_packet_count 
        << " ms\n" // Среднее время обслуживания пакета
        << "Retried packet count = " // Количество пакетов обслуженных не с первого раза и их доля
        << average_total_retried_packet_count 
        << " (" << ((float)average_total_retried_packet_count / average_total_packet_count * 100) 
        << "% of all)\n"
        << "Packet loss = " // Количество пакетов обслуженных не с первого раза и их доля
        << average_total_lost_packet_count << " (" 
        << ((float)average_total_lost_packet_count / (average_total_packet_count + average_total_lost_packet_count) * 100) 
        << "% of all)\n\n";
}

// Рисование графика средних задержек по очередям для всех запусков
void AverageStats::draw_delay_plot(){
    std::vector<int> x_ticks = {};
    std::vector<std::string> x_labels = {};

    // Подписи к графикам
    for (size_t i = 0; i < average_delays.size(); ++i){
        x_ticks.push_back(i);

        std::string label = "queue " + std::to_string(i + 1);
        x_labels.push_back(label);
    }
    
    // Построение графика
    plt::bar(average_delays, "black", "-", 0.5);
    plt::xticks(x_ticks, x_labels);
    plt::title("Средняя задержка обслуживания пакетов");
    plt::xlabel("Название очереди");
    plt::ylabel("Задержка (мс) ");
    std::string filename = "./average_delay.png";
    std::cout << "Saving result to " << filename << std::endl;
    plt::save(filename);
    plt::close();
}

// Рисование графика средних количества пакетов пришедших в очереди для всех запусков
void AverageStats::draw_scheduling_plot(){
    // Подписи к графикам
    for (auto& queue_stats : average_packets_scheduled_by_ms){
        std::string label = "Queue " + std::to_string(queue_stats.first + 1);
        plt::named_plot(label, queue_stats.second);
    }

    // Построение графика
    plt::title("Время прихода пакетов по очередям");
    plt::ylabel("Среднее количество пакетов");
    plt::xlabel("Время прихода пакета относительно запуска (мс) ");
    plt::legend();
    std::string filename = "./average_scheduling.png";
    std::cout << "Saving result to " << filename << std::endl;;
    plt::save(filename);
    plt::close();
}