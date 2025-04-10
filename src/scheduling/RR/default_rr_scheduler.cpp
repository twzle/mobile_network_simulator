#include "scheduling/RR/default_rr_scheduler.hpp"

DefaultRRScheduler::DefaultRRScheduler()
    : BaseRRScheduler() {};

/*
Логика работы планировщика
*/
void DefaultRRScheduler::run()
{
    // Метка времени в момент запуска планировщика
    session.set_scheduling_start_time(0.0);
    double current_time = session.get_scheduling_start_time();

    // Перебор очередей начиная с очереди с индексом 0
    set_initial_queue(0);

    // Цикл до обслуживания всех пакетов во всех очередях
    while (session.get_processed_packet_count() < this->total_packets)
    {
        // Начало TTI
        reset_served_users();
        sync_user_channels();

        SchedulerState scheduler_state = SchedulerState::UNDEFINED;

        int available_resource_blocks = this->resource_blocks_per_tti;
        set_last_starving_queue(0); // Последняя недообслуженная очередь

        for (size_t absolute_queue_id = 0;
             absolute_queue_id < scheduled_queues.size();
             ++absolute_queue_id)
        {
            PacketQueueState queue_state = PacketQueueState::UNDEFINED;

            size_t relative_queue_id = get_relative_queue_id(absolute_queue_id);

            PacketQueue &queue = scheduled_queues[relative_queue_id];

            // std::cout << "-> " << relative_queue_id << " ";

            if (queue.size() == 0)
            {
                queue_state = set_wait(queue_state);
                scheduler_state = set_wait(scheduler_state);
            }
            else
            {
                while (queue.size() > 0)
                {
                    // Проверка первого пакета в очереди
                    Packet packet = queue.front();
                    int packet_size_in_bytes = packet.get_size();
                    int packet_size_in_rb =
                        convert_packet_size_to_rb_number(
                            packet.get_user_ptr(), packet_size_in_bytes);

                    // Если пакет пришел позже текущего времени, то переход к следующей очереди
                    if (packet.get_scheduled_at() > current_time + epsilon)
                    {
                        queue_state = set_wait(queue_state);
                        scheduler_state = set_wait(scheduler_state);
                        break;
                    }

                    // Если размер пакета больше, чем реальное кол-во RB, то переход к следующей очереди
                    if (packet_size_in_rb > available_resource_blocks)
                    {
                        // Последняя очередь на которую не хватило RB
                        set_last_starving_queue(relative_queue_id);

                        queue_state = set_idle(queue_state);
                        scheduler_state = set_idle(scheduler_state);

                        // Кандидаты на получение ресурсов пользователь и очередь
                        mark_as_resource_candidate(
                            packet.get_queue(), 
                            packet.get_user_ptr()
                        );

                        break;
                    }

                    // Если лимит обслуженных пользователей достигнут
                    if (users_served_in_tti.size() == (size_t)users_per_tti_limit)
                    {
                        auto it = users_served_in_tti.find(packet.get_user_ptr());

                        // Не найден в списке пользователей на текущий TTI
                        if (it == users_served_in_tti.end())
                        {
                            queue_state = set_idle(queue_state);
                            scheduler_state = set_idle(scheduler_state);

                            // Кандидаты на получение ресурсов пользователь и очередь
                            mark_as_resource_candidate(
                                packet.get_queue(), 
                                packet.get_user_ptr()
                            );
    
                            break;
                        }
                    }

                    // Если размер пакета меньше, чем реальное кол-во RB, то обслуживаем пакет
                    if (packet_size_in_rb <= available_resource_blocks)
                    {
                        // Обслуживание пакета
                        queue.pop();

                        users_served_in_tti.insert(packet.get_user_ptr());

                        session.increment_processed_packet_count(1);

                        available_resource_blocks -= packet_size_in_rb;

                        // Кандидаты на получение ресурсов пользователь и очередь
                        mark_as_resource_candidate(
                            packet.get_queue(), 
                            packet.get_user_ptr()
                        );

                        save_processed_packet_stats(
                            packet,
                            packet_size_in_bytes,
                            packet_size_in_rb,
                            current_time
                        );

                        queue_state = set_processing(queue_state);
                        scheduler_state = set_processing(scheduler_state);
                    }
                }
            }

            check_queue_remaining_scheduled_packets(
                queue, current_time);

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

        evaluate_fairness_stats(scheduler_state, false);
        evaluate_throughput_stats(false);

        // Обновление начальной очереди
        set_initial_queue(get_next_initial_queue());
    }

    // Метка времени в момент завершения работы планировщика
    session.set_scheduling_end_time(current_time);

    // Подсчет статистики
    evaluate_stats();
}

// Начало следующего TTI всегда с последней недообслуженной очереди
int DefaultRRScheduler::get_next_initial_queue()
{
    return last_starving_queue;
}

// Сохранение последней недообслуженной очереди
void DefaultRRScheduler::set_last_starving_queue(int last_starving_queue)
{
    this->last_starving_queue = last_starving_queue;
}