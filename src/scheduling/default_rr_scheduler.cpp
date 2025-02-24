#include "scheduling/default_rr_scheduler.hpp"

DefaultRRScheduler::DefaultRRScheduler(
    std::string standard_name, double tti,
    double channel_sync_interval,
    uint8_t base_cqi)
    : BaseRRScheduler(
          standard_name, tti,
          channel_sync_interval,
          base_cqi) {};

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
        TTIStats tti_stats = TTIStats(
            standard_name,
            scheduled_queues.size(),
            connected_users.size(),
            tti_duration);

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
                        tti_stats.mark_user_as_resource_candidate(packet.get_user_ptr());
                        tti_stats.mark_queue_as_resource_candidate(packet.get_queue());
                        break;
                    }

                    // Если размер пакета меньше, чем реальное кол-во RB, то обслуживаем пакет
                    if (packet_size_in_rb <= available_resource_blocks)
                    {
                        // Обслуживание пакета
                        queue.pop();
                        session.increment_processed_packet_count(1);

                        available_resource_blocks -= packet_size_in_rb;

                        // Кандидаты на получение ресурсов пользователь и очередь
                        tti_stats.mark_user_as_resource_candidate(packet.get_user_ptr());
                        tti_stats.mark_queue_as_resource_candidate(packet.get_queue());

                        tti_stats.add_allocated_effective_data_to_queue(
                            packet.get_user_ptr(),
                            packet.get_queue(),
                            packet_size_in_rb);

                        tti_stats.add_allocated_effective_data_to_user(
                            packet.get_user_ptr(),
                            packet_size_in_rb);

                        tti_stats.add_allocated_effective_data_to_total(
                            packet.get_user_ptr(),
                            packet_size_in_rb);

                        stats.add_queue_packet_stats(
                            packet.get_queue(),
                            current_time - packet.get_scheduled_at());

                        queue_state = set_processing(queue_state);
                        scheduler_state = set_processing(scheduler_state);
                    }
                }
            }

            check_queue_remaining_scheduled_packets(
                queue, current_time, tti_stats);

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

        tti_stats.calculate_fairness_for_queues();
        stats.update_scheduler_fairness_for_queues(
            tti_stats.get_fairness_for_queues(),
            tti_stats.is_valid_fairness_for_queues());

        tti_stats.calculate_fairness_for_users();
        stats.update_scheduler_fairness_for_users(
            tti_stats.get_fairness_for_users(),
            tti_stats.is_valid_fairness_for_users());

        tti_stats.calculate_throughput_for_scheduler();
        stats.update_scheduler_throughput(
            tti_stats.get_throughput_for_scheduler(),
            tti_stats.is_valid_throughput_for_scheduler());

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