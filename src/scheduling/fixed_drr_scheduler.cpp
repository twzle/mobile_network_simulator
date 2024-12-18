#include <string>
#include "scheduling/fixed_drr_scheduler.hpp"

FixedDRRScheduler::FixedDRRScheduler(double tti)
    : BaseDRRScheduler(tti) {};

/*
Логика работы планировщика
*/
void FixedDRRScheduler::run()
{
    // Метка времени в момент запуска планировщика
    set_scheduling_start_time(0.0);
    double current_time = get_scheduling_start_time();

    size_t initial_relative_queue_id_for_next_tti = 0; // Начало следующего TTI всегда с 0 очереди
    set_initial_queue(initial_relative_queue_id_for_next_tti);

    // Цикл до обслуживания всех пакетов во всех очередях
    while (this->processed_packets < this->total_packets)
    {
        // Начало TTI
        int avaialable_resource_blocks = this->resource_blocks_per_tti;

        for (size_t absolute_queue_id = 0;
             absolute_queue_id < scheduled_queues.size();
             ++absolute_queue_id)
        {
            size_t relative_queue_id = get_relative_queue_id(absolute_queue_id);

            // std::cout << "-> " << relative_queue_id << " ";

            PacketQueue &queue = scheduled_queues[relative_queue_id];
            queue.set_deficit(queue.get_deficit() + queue.get_quant());

            if (queue.size() == 0)
            {
                continue;
            }

            while (queue.size() > 0)
            {
                // Обслуживание первого в очереди пакета
                Packet packet = queue.front();

                if (current_time + epsilon < packet.get_scheduled_at())
                {
                    break;
                }

                if (packet.get_size() > queue.get_deficit())
                {
                    break;
                }

                if (packet.get_size() > avaialable_resource_blocks)
                {
                    break;
                }

                // Проверка достаточности дефицита на обслуживание пакета
                if (packet.get_size() <= avaialable_resource_blocks)
                {
                    // Обслуживание пакета
                    queue.pop();
                    queue.set_deficit(queue.get_deficit() - packet.get_size());
                    avaialable_resource_blocks -= packet.get_size();

                    increment_processed_packet_count(1);

                    stats.add_packet_stats_for_queue(
                        relative_queue_id,
                        packet.get_scheduled_at(),
                        current_time);

                    stats.increment_active_processing_time_for_queue(
                        relative_queue_id,
                        tti_duration);

                    if (queue.size() == 0)
                    {
                        stats.set_total_processing_time_for_queue(
                            relative_queue_id,
                            current_time);
                    }
                }
            }
        }
        // Конец TTI
        current_time += this->tti_duration;
    }

    // Метка времени в момент завершения работы планировщика
    set_scheduling_end_time(current_time);
    scheduling_duration = get_scheduling_end_time() - get_scheduling_start_time();

    // Подсчет статистики
    evaluate_stats();
}