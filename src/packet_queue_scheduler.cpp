#include <string>
#include "packet_queue_scheduler.hpp"


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
            // Перебор пакетов только в наполненной очереди
            while (queue.size() > 0)
            {
                // Обслуживание первого в очереди пакета
                Packet packet = queue.front();
                ms packet_size = packet.get_size();
                ms deficit = queue.get_deficit();
                time_point packet_scheduled_at = packet.get_scheduled_at();

                if (packet_scheduled_at > SystemClock::now())
                {
                    break;
                }
                else
                {
                    queue.set_deficit(queue.get_deficit() + queue.get_quant());
                }

                // Проверка достаточности дефицита на обслуживание пакета
                if (packet_size <= deficit)
                {
                    // Возможный процесс обслуживания пакета с вычислением длительности
                    auto packet_processing_start =
                        std::chrono::high_resolution_clock::now();
                    ms sleep_duration(packet_size); // Заглушка на время обслуживания пакета
                    std::this_thread::sleep_for(sleep_duration);
                    auto packet_processing_end =
                        std::chrono::high_resolution_clock::now();

                    if (queue.front().get_retry())
                    {
                        stats.retried_packet_count++;
                    }

                    queue.pop();
                    ++served_packets;

                    ms packet_processing_duration =
                        std::chrono::duration_cast<ms>
                        (packet_processing_end - packet_processing_start);
                    ms packet_delay_duration =
                        std::chrono::duration_cast<ms>
                        (packet_processing_end - packet_scheduled_at);
                    ms packet_scheduled_at_ms =
                        std::chrono::duration_cast<ms>
                        (packet_scheduled_at.time_since_epoch());


                    stats.total_processing_time += packet_processing_duration;
                    stats.queue_stats[queue_id].push_back(
                        PacketStats(
                            packet_scheduled_at_ms,
                            packet_delay_duration));

                    // Вычисление фактического дефицита по результатам времени обслуживания
                    queue.set_deficit(deficit - packet_processing_duration);
                }
                else
                {
                    // Принудительная смена очереди из-за нехватки накопленного дефицита
                    Packet packet = queue.front();
                    packet.set_retry();
                    queue.pop();
                    queue.push(packet);
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
    stats.draw_scheduling_plot();
    stats.draw_delay_plot();
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