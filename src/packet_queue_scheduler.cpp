#include "packet_queue_scheduler.hpp"
#include <matplotlibcpp.h>
#include <string>
namespace plt = matplotlibcpp;

PacketQueueScheduler::PacketQueueScheduler(PacketQueue &packet_queue)
{
    this->schedule(packet_queue);
}

/*
Логика работы планировщика
*/
void PacketQueueScheduler::run()
{
    // Метка времени в момент запуска планировщика
    auto run_start = std::chrono::high_resolution_clock::now();
    int served_packets = 0; // Счетчик обслуженных пакетов

    // Цикл до обслуживания всех пакетов во всех очередях
    while (served_packets < this->total_packets)
    {
        int queue_id = 0;

        for (auto &queue : scheduled_queues)
        {
            std::cout << "Queue #" << queue_id + 1 << "\n";
            queue.set_deficit(queue.get_deficit() + queue.get_quant());

            // Перебор пакетов только в наполненной очереди
            while (queue.size() > 0)
            {
                // Обслуживание первого в очереди пакета
                time_point packet_scheduled_at = queue.front().get_scheduled_at();
                ms packet_size = queue.front().get_size();
                ms deficit = queue.get_deficit();

                // Проверка достаточности дефицита на обслуживание пакета
                if (packet_size <= deficit)
                {
                    // Возможный процесс обслуживания пакета с вычислением длительности
                    auto packet_processing_start =
                        std::chrono::high_resolution_clock::now();
                    ms sleep_duration(packet_size); // Заглушка на время обслуживания пакета
                    std::this_thread::sleep_for(sleep_duration);
                    auto packet_processing_end = std::chrono::high_resolution_clock::now();

                    if (queue.front().get_retry())
                    {
                        stats.retried_packet_count++;
                    }

                    queue.pop();
                    ++served_packets;
                    queue.set_deficit(deficit - packet_size);

                    auto packet_processing_duration =
                        std::chrono::duration_cast<ms>(packet_processing_end - packet_processing_start);
                    auto packet_delay_duration =
                        std::chrono::duration_cast<ms>(packet_processing_start - packet_scheduled_at);

                    stats.total_processing_time += packet_processing_duration;
                    stats.packet_processing_delay[queue_id].push_back(packet_delay_duration);

                    // Вычисление фактического дефицита по результатам времени обслуживания
                    queue.set_deficit(deficit - packet_processing_duration);

                    std::cout << "Packet of size " << packet_size.count()
                              << " served in " << packet_processing_duration.count() << " ms."
                              << " Remaining deficit: " << queue.get_deficit().count()
                              << "\n";
                }
                else
                {
                    // Принудительная смена очереди из-за нехватки накопленного дефицита
                    queue.front().set_retry();
                    break;
                }
            }

            queue_id++;
        }
    }

    // Метка времени в момент завершения работы планировщика
    auto run_end = std::chrono::high_resolution_clock::now();
    auto run_duration =
        std::chrono::duration_cast<ms>(run_end - run_start);

    // Подсчет статистики
    stats.total_time = run_duration;
    stats.packet_count = total_packets;

    for (auto &queue : scheduled_queues)
    {
        stats.lost_packet_count += queue.get_lost_packet_count();
    }

    stats.summarize();
}

/*
Планирование очереди через запись в массив очередей
и вычисление новой суммы общего количества пакетов
*/
void PacketQueueScheduler::schedule(const PacketQueue &packet_queue)
{
    scheduled_queues.push_back(packet_queue);
    total_packets += packet_queue.size();
}

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
              << lost_packet_count << " (" << ((float)lost_packet_count / packet_count * 100) << "% of all)\n";

    draw_delay_plot();
}

// Рисование графика задержек
void Stats::draw_delay_plot()
{
    std::vector<float> average_delays;
    std::vector<int> x_ticks = {};
    std::vector<std::string> x_labels = {};
    for (auto &delay_vector : packet_processing_delay)
    {
        std::cout << "Queue #" << delay_vector.first + 1 << " processing timings in ms\n";
        x_ticks.push_back(delay_vector.first);
        std::string label = "queue " + std::to_string(delay_vector.first + 1);
        x_labels.push_back(label);

        int total_delay = 0;
        for (auto &delay : delay_vector.second)
        {
            std::cout << (int) delay.count() << " ";
            total_delay += (int) delay.count();
        }

        float average_delay = (float) total_delay / delay_vector.second.size();
        average_delays.push_back(average_delay);
        std::cout << "\nAverage = " << average_delay << "\n";
    }

    plt::bar(average_delays, "black", "-", 0.5);

    plt::xticks(x_ticks, x_labels);

    plt::title("Средняя задержка обслуживания пакетов");
    plt::xlabel("Название очереди");
    plt::ylabel("Задержка (мс) ");
    
    const char* filename = "./bar.png";
    std::cout << "Saving result to " << filename << std::endl;;
    plt::save(filename);
}