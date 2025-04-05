#include "scheduling/DRR/fixed_drr_scheduler.hpp"

FixedDRRScheduler::FixedDRRScheduler()
    : BaseRRScheduler() {};

/*
Логика работы планировщика
*/
void FixedDRRScheduler::run()
{
    // Метка времени в момент запуска планировщика
    session.set_scheduling_start_time(tti_duration);
    double current_time = session.get_scheduling_start_time();

    // Начало следующего TTI всегда с 0 очереди
    set_initial_queue(0);

    // Цикл до обслуживания всех пакетов во всех очередях
    while (session.get_processed_packet_count() < this->total_packets)
    {
        // Начало TTI
        TTIStats tti_stats = TTIStats(
            scheduled_queues.size(),
            connected_users.size(),
            tti_duration);

        reset_served_users();
        sync_user_channels();

        SchedulerState scheduler_state = SchedulerState::UNDEFINED;

        int available_resource_blocks = this->resource_blocks_per_tti;

        for (size_t absolute_queue_id = 0;
             absolute_queue_id < scheduled_queues.size();
             ++absolute_queue_id)
        {
            PacketQueueState queue_state = PacketQueueState::UNDEFINED;

            size_t relative_queue_id = get_relative_queue_id(absolute_queue_id);

            PacketQueue &queue = scheduled_queues[relative_queue_id];
            queue.set_deficit(queue.get_deficit() + queue.get_quant());

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

                    // Если размер пакета больше, чем дефицит, то переход к следующей очереди
                    if (packet_size_in_rb > queue.get_deficit() + epsilon)
                    {
                        queue_state = set_idle(queue_state);
                        scheduler_state = set_idle(scheduler_state);

                        // Кандидаты на получение ресурсов пользователь и очередь, но
                        // не хватило дефицита
                        tti_stats.mark_queue_as_resource_candidate(packet.get_queue());
                        tti_stats.mark_user_as_resource_candidate(packet.get_user_ptr());
                        break;
                    }

                    // Если размер пакета больше, чем реальное кол-во RB, то переход к следующей очереди
                    if (packet_size_in_rb > available_resource_blocks)
                    {
                        queue_state = set_idle(queue_state);
                        scheduler_state = set_idle(scheduler_state);

                        // Кандидаты на получение ресурсов пользователь и очередь, но 
                        // не хватило канала 
                        tti_stats.mark_user_as_resource_candidate(packet.get_user_ptr());
                        tti_stats.mark_queue_as_resource_candidate(packet.get_queue());
                        break;
                    }

                    // Если лимит обслуженных пользователей достигнут
                    if (users_served_in_tti.size() == (size_t) users_per_tti_limit)
                    {
                        auto it = users_served_in_tti.find(packet.get_user_ptr());

                        // Не найден в списке пользователей на текущий TTI
                        if (it == users_served_in_tti.end())
                        {
                            queue_state = set_idle(queue_state);
                            scheduler_state = set_idle(scheduler_state);

                            // Кандидаты на получение ресурсов пользователь и очередь, но
                            // ограничены лимитом пользователей в TTI
                            tti_stats.mark_user_as_resource_candidate(packet.get_user_ptr());
                            tti_stats.mark_queue_as_resource_candidate(packet.get_queue());
                            break;
                        }
                    }

                    // Если размер пакета меньше, чем реальное кол-во RB, то обслуживаем пакет
                    if (packet_size_in_rb <= available_resource_blocks)
                    {
                        // Обслуживание пакета
                        queue.pop();
                        queue.set_deficit(queue.get_deficit() - packet_size_in_rb);

                        users_served_in_tti.insert(packet.get_user_ptr());

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
                            packet.get_user_ptr()->get_id(),
                            current_time - packet.get_scheduled_at());

                        queue_state = set_processing(queue_state);
                        scheduler_state = set_processing(scheduler_state);
                    }
                }
            }

            check_queue_remaining_scheduled_packets_with_queue_quant(
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

// Перебор в следующем TTI фиксированно c 0 очереди
int FixedDRRScheduler::get_next_initial_queue()
{
    return 0;
}