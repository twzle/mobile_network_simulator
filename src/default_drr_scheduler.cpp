#include <string>
#include "default_drr_scheduler.hpp"

std::map<TTI, double> tti_values = {
    {TTI::LTE, 0.001}, // 1 мс
};

DefaultDRRScheduler::DefaultDRRScheduler()
    : tti_duration(tti_values[TTI::LTE]) {}

/*
Планирование очереди через запись в массив очередей
и вычисление новой суммы общего количества пакетов
*/
void DefaultDRRScheduler::schedule(PacketQueue &&packet_queue)
{
    scheduled_queues.push_back(packet_queue);
    total_packets += packet_queue.size();
}

/*
Логика работы планировщика
*/
void DefaultDRRScheduler::run()
{
    // Метка времени в момент запуска планировщика
    double scheduling_start = 0.0;
    double current_time = scheduling_start;
    int processed_packets_count = 0; // Счетчик обслуженных пакетов

    // Цикл до обслуживания всех пакетов во всех очередях
    while (processed_packets_count < this->total_packets)
    {
        // Начало TTI
        int avaialable_resource_blocks = this->resource_blocks_per_tti;
        size_t last_starving_queue = 0; // Последняя недообслуженная очередь

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
                    last_starving_queue = relative_queue_id;
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
        set_initial_queue(last_starving_queue);
        current_time += this->tti_duration;
    }

    // Метка времени в момент завершения работы планировщика
    double scheduling_end = current_time;
    scheduling_duration = scheduling_end - scheduling_start;

    // Подсчет статистики
    evaluate_stats();
}

void DefaultDRRScheduler::set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit){
    this->resource_blocks_per_tti = resource_blocks_per_tti_limit;
}

void DefaultDRRScheduler::set_initial_queue(int new_initial_queue_id){
    this->current_initial_absolute_queue_id = new_initial_queue_id;
}

int DefaultDRRScheduler::get_initial_queue()
{
    return this->current_initial_absolute_queue_id;
}

size_t DefaultDRRScheduler::get_relative_queue_id(size_t current_absolute_queue_id)
{
    size_t bias = this->current_initial_absolute_queue_id;
    size_t current_relative_queue_id = current_absolute_queue_id + bias;

    if (current_relative_queue_id >= scheduled_queues.size())
    {
        return current_relative_queue_id % scheduled_queues.size();
    }
    else
    {
        return current_relative_queue_id;
    }
}

/*
Подсчет статистики по результатам работы планировщика
*/
void DefaultDRRScheduler::evaluate_stats()
{
    stats.total_time = scheduling_duration;
    stats.packet_count = total_packets;

    for (auto &queue : scheduled_queues)
    {
        stats.lost_packet_count += queue.get_lost_packet_count();
    }
}


ExecutionStats &DefaultDRRScheduler::get_stats()
{
    return this->stats;
}