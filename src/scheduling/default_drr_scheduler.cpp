#include <string>
#include "scheduling/default_drr_scheduler.hpp"

DefaultDRRScheduler::DefaultDRRScheduler(double tti)
    : BaseDRRScheduler(tti) {};

/*
Логика работы планировщика
*/
void DefaultDRRScheduler::run()
{
    // Метка времени в момент запуска планировщика
    set_scheduling_start_time(0.0);
    double current_time = get_scheduling_start_time();

    // Перебор очередей начиная с очереди с индексом 0
    set_initial_queue(0);

    // Цикл до обслуживания всех пакетов во всех очередях
    while (this->processed_packets < this->total_packets)
    {
        // Начало TTI
        SchedulerState scheduler_state = SchedulerState::UNDEFINED;

        int available_resource_blocks = this->resource_blocks_per_tti;
        int allocated_resource_blocks_for_tti = 0;
        set_last_starving_queue(0); // Последняя недообслуженная очередь

        for (size_t absolute_queue_id = 0;
             absolute_queue_id < scheduled_queues.size();
             ++absolute_queue_id)
        {
            QueueState queue_state = QueueState::UNDEFINED;

            size_t relative_queue_id = get_relative_queue_id(absolute_queue_id);

            PacketQueue &queue = scheduled_queues[relative_queue_id];
            queue.set_deficit(queue.get_deficit() + queue.get_quant());

            // std::cout << "-> " << relative_queue_id << " ";

            int allocated_resource_blocks_for_queue = 0;

            if (queue.size() == 0)
            {
                queue_state = set_wait(queue_state);
                scheduler_state = set_wait(scheduler_state);
            }
            else
            {
                while (queue.size() > 0)
                {
                    // Обслуживание первого в очереди пакета
                    Packet packet = queue.front();

                    if (current_time + epsilon < packet.get_scheduled_at())
                    {
                        queue_state = set_wait(queue_state);
                        scheduler_state = set_wait(scheduler_state);
                        break;
                    }

                    if (packet.get_size() > queue.get_deficit())
                    {
                        queue_state = set_idle(queue_state);
                        scheduler_state = set_idle(scheduler_state);
                        break;
                    }

                    if (packet.get_size() > available_resource_blocks)
                    {
                        // Последняя очередь на которую не хватило RB
                        set_last_starving_queue(relative_queue_id);

                        queue_state = set_idle(queue_state);
                        scheduler_state = set_idle(scheduler_state);
                        break;
                    }

                    // Проверка достаточности дефицита на обслуживание пакета
                    if (packet.get_size() <= available_resource_blocks)
                    {
                        // Обслуживание пакета
                        queue.pop();
                        queue.set_deficit(queue.get_deficit() - packet.get_size());
                        increment_processed_packet_count(1);

                        available_resource_blocks -= packet.get_size();
                        allocated_resource_blocks_for_queue += packet.get_size();

                        stats.add_queue_packet_stats(
                            relative_queue_id,
                            packet.get_scheduled_at(),
                            current_time);

                        queue_state = set_processing(queue_state);
                        scheduler_state = set_processing(scheduler_state);
                    }
                }
            }

            allocated_resource_blocks_for_tti += allocated_resource_blocks_for_queue;

            stats.update_queue_time_stats(
                queue_state,
                relative_queue_id,
                tti_duration);
        }
        // Конец TTI
        current_time += this->tti_duration;

        stats.update_scheduler_time_stats(
            scheduler_state,
            tti_duration);

        // Начало следующего TTI всегда с последней недообслуженной очереди
        set_initial_queue(get_next_initial_queue());
    }

    // Метка времени в момент завершения работы планировщика
    set_scheduling_end_time(current_time);

    // Подсчет статистики
    evaluate_stats();
}

int DefaultDRRScheduler::get_next_initial_queue()
{
    return last_starving_queue;
}

void DefaultDRRScheduler::set_last_starving_queue(int last_starving_queue)
{
    this->last_starving_queue = last_starving_queue;
}