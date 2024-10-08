#include <matplotlibcpp.h>
#include "stats.hpp"
#include "time_generator.hpp"

namespace plt = matplotlibcpp;

PacketStats::PacketStats(ms scheduled_at, ms processing_delay)
    : scheduled_at(scheduled_at), processing_delay(processing_delay) {}

Stats::Stats() : total_time(0), total_skip_time(0),
                 total_processing_time(0), packet_count(0),
                 retried_packet_count(0), lost_packet_count(0) {}

// Вывод статистики в stdout
void Stats::summarize()
{
    // Общее время простоя
    auto total_idle_time = total_time.count() - total_processing_time.count();

    std::cout << "\nTotal running time = "
              << total_time.count() << " ms\n" // Общее время работы
              << "Total processing time = "
              << total_processing_time.count() << " ms (" // Общее время обслуживания пакетов и доля от общего времени
              << 100 * ((float)total_processing_time.count() / (float)total_time.count()) << "% of all)\n"
              << "Total idle time = "
              << total_idle_time << " ms (" // Общее время простоя и доля от общего времени
              << 100 * ((float)total_idle_time / (float)total_time.count()) << "% of all)\n"
              << "Average packet processing time = "
              << total_processing_time.count() / packet_count << " ms\n" // Среднее время обслуживания пакета
              << "Retried packet count = "                               // Количество пакетов обслуженных не с первого раза и их доля
              << retried_packet_count << " (" << ((float)retried_packet_count / packet_count * 100) << "% of all)\n"
              << "Packet loss = " // Количество пакетов обслуженных не с первого раза и их доля
              << lost_packet_count << " (" << ((float)lost_packet_count / (packet_count + lost_packet_count) * 100) << "% of all)\n\n";
}

// Рисование графика задержек
void Stats::draw_delay_plot()
{
    std::vector<float> average_delays;
    std::vector<int> x_ticks = {};
    std::vector<std::string> x_labels = {};

    for (auto &stats : queue_stats)
    {
        // std::cout << "Queue #"
        //           << stats.first + 1
        //           << " processing timings in ms\n";
        x_ticks.push_back(stats.first);

        std::string label = "queue " + std::to_string(stats.first + 1);
        x_labels.push_back(label);

        int total_delay = 0;
        for (auto &packet_stats : stats.second)
        {
            // std::cout << (int)packet_stats.processing_delay.count() << " ";
            total_delay += (int)packet_stats.processing_delay.count();
        }

        float average_delay = (float)total_delay / stats.second.size();
        average_delays.push_back(average_delay);
        // std::cout << "\nAverage = " << average_delay << "\n";
    }

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

// Рисование графика планирования
void Stats::draw_scheduling_plot()
{
    ms initial_scheduling_time = TimeGenerator::get_initial_time();

    std::map<int, std::map<int, int>> relative_scheduling_time;

    for (auto &stats : queue_stats)
    {
        // std::cout << "Queue #" << stats.first << "\n";
        for (auto &packet_stats : stats.second)
        {
            int relative_packet_schedule_time =
                (packet_stats.scheduled_at - initial_scheduling_time).count();
            // std::cout << relative_packet_schedule_time << "\n";
            relative_scheduling_time[stats.first]
                                    [relative_packet_schedule_time]++;
        }
        // std::cout << "\n";
    }

    // for (auto &queue_stats : relative_scheduling_time)
    // {
    //     std::cout << "Queue #" << queue_stats.first << "\n";
    //     for (auto &schedule_pair : queue_stats.second)
    //     {
    //         std::cout << "Value (ms): " << schedule_pair.first
    //                   << " Packets scheduled: " << schedule_pair.second << "\n";
    //     }
    //     std::cout << "\n";
    // }

    int lower_limit = TimeGenerator::get_distribution().a();
    int upper_limit = TimeGenerator::get_distribution().b();

    int n = upper_limit - lower_limit + 1;
    std::map<int, std::vector<int>> packets_scheduled_by_ms;

    for (auto &queue_stats : relative_scheduling_time)
    {   
        packets_scheduled_by_ms[queue_stats.first].resize(n);
        for (int i = 0; i < n; ++i)
        {
            auto it = relative_scheduling_time[queue_stats.first].find(i);
            if (it != relative_scheduling_time[queue_stats.first].end())
            {
                // Записываем количество повторов
                packets_scheduled_by_ms[queue_stats.first].at(i) = it->second;
            }
        }
    }

    for (auto& queue_stats : packets_scheduled_by_ms){
        // std::cout << "Queue #" << queue_stats.first << " vector:\n";
        // for (auto& packet_count : queue_stats.second){
            // std::cout << packet_count << " ";
        // }
        std::string label = "Queue " + std::to_string(queue_stats.first + 1);
        plt::named_plot(label, queue_stats.second);
        
        // std::cout << "\n";
    }

    plt::title("Время прихода пакетов по очередям");
    plt::ylabel("Количество пакетов");
    plt::xlabel("Время прихода пакета относительно запуска (мс) ");
    plt::legend();
    std::string filename = "./scheduling.png";
    std::cout << "Saving result to " << filename << std::endl;;
    plt::save(filename);
    plt::close();
}