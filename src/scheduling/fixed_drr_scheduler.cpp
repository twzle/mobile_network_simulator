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
    double scheduling_start = 0.0;
    double current_time = scheduling_start;
    int processed_packets_count = 0; // Счетчик обслуженных пакетов
    
    size_t initial_relative_queue_id_for_next_tti = 0; // Начало следующего TTI всегда с 0 очереди
    set_initial_queue(initial_relative_queue_id_for_next_tti);

    // Цикл до обслуживания всех пакетов во всех очередях
    while (processed_packets_count < this->total_packets)
    {
        // Начало TTI
        int avaialable_resource_blocks = this->resource_blocks_per_tti;

        for (size_t absolute_queue_id = 0; 
            absolute_queue_id < scheduled_queues.size(); 
            ++absolute_queue_id)
        {
            size_t relative_queue_id = get_relative_queue_id(absolute_queue_id);

            // std::cout << "-> " << relative_queue_id << " ";

            PacketQueue& queue = scheduled_queues[relative_queue_id];
            queue.set_deficit(queue.get_deficit() + queue.get_quant());

            double queue_processing_start_time = current_time;

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
                
                if (packet.get_size() > avaialable_resource_blocks){
                    break;
                }

                // Проверка достаточности дефицита на обслуживание пакета
                if (packet.get_size() <= avaialable_resource_blocks)
                {
                    // Обслуживание пакета
                    queue.pop();
                    queue.set_deficit(queue.get_deficit() - packet.get_size());
                    avaialable_resource_blocks -= packet.get_size();

                    ++processed_packets_count;

                    double packet_processing_duration = packet.get_size();
                    double packet_delay_duration = current_time - packet.get_scheduled_at();

                    stats.total_processing_time += packet_processing_duration;
                    stats.queue_stats[relative_queue_id].emplace_back(
                        packet.get_scheduled_at(),
                        packet_delay_duration);
                }
            }
            double queue_processing_end_time = current_time;
            double queue_processing_duration =
                queue_processing_end_time - queue_processing_start_time;

            stats.processing_time_by_queue[relative_queue_id] += 
                queue_processing_duration;
        }
        // Конец TTI
        current_time += this->tti_duration;
    }

    // Метка времени в момент завершения работы планировщика
    double scheduling_end = current_time;
    scheduling_duration = scheduling_end - scheduling_start;

    // Подсчет статистики
    evaluate_stats();
}