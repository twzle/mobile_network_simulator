#include "scheduling/PF/base_pf_scheduler.hpp"

BasePFScheduler::BasePFScheduler() : BaseScheduler() {}

/*
Планирование очереди через запись в массив очередей
и вычисление новой суммы общего количества пакетов
*/
void BasePFScheduler::schedule(PacketQueue &&packet_queue)
{
    total_packets += packet_queue.size();
    main_queue = std::move(packet_queue);
}

/*
Логика работы планировщика
*/
void BasePFScheduler::run()
{
    // Метка времени в момент запуска планировщика
    session.set_scheduling_start_time(tti_duration);
    double current_time = session.get_scheduling_start_time();

    // Цикл до обслуживания всех пакетов во всех очередях
    while (session.get_processed_packet_count() < this->total_packets)
    {
        // Начало TTI

        reset_served_users();
        flush_user_context();
        sync_user_channels();
        update_user_priorities();

        collect_relevant_packets(current_time);
        sort_users();
        exclude_users_from_scheduling();
        filter_packets_of_excluded_from_scheduling_users();

        int available_resource_blocks = this->resource_blocks_per_tti;

        SchedulerState scheduler_state = SchedulerState::UNDEFINED;
        PacketQueueState queue_state = PacketQueueState::UNDEFINED;

        queue_state = set_wait(queue_state);
        scheduler_state = set_wait(scheduler_state);

        while (relevant_queue.size() > 0)
        {
            Packet packet = relevant_queue.front();

            int packet_size_in_bytes = packet.get_size();
            int packet_size_in_rb =
                convert_packet_size_to_rb_number(
                    packet.get_user_ptr(), packet_size_in_bytes);

            // Не хватает RB на обслуживание
            if (packet_size_in_rb > available_resource_blocks)
            {
                queue_state = set_idle(queue_state);
                scheduler_state = set_idle(scheduler_state);

                // Кандидаты на получение ресурсов пользователь и очередь
                mark_as_resource_candidate(
                    packet.get_queue(), 
                    packet.get_user_ptr()
                );

                // Возврат в исходную очередь вместо обслуживания
                relevant_queue.pop();
                main_queue.push(packet);
            }

            // Хватает RB на обслуживание пакета
            if (packet_size_in_rb <= available_resource_blocks)
            {
                // Обслуживание пакета без возврата в исходную очередь
                relevant_queue.pop();
                packet.get_user_ptr()->increment_current_throughput(
                    packet_size_in_rb);

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
                    current_time);

                queue_state = set_processing(queue_state);
                scheduler_state = set_processing(scheduler_state);
            }
        }

        // Конец TTI
        current_time += this->tti_duration;

        update_user_throughputs();

        stats.update_scheduler_time_stats(
            scheduler_state,
            tti_duration);

        evaluate_fairness_stats(scheduler_state, false);
        evaluate_throughput_stats(false);
    }

    // Метка времени в момент завершения работы планировщика
    session.set_scheduling_end_time(current_time);

    // Подсчет статистики
    evaluate_stats();
}

// Фильтрация актуальных пакетов по времени прихода на текущий момент
void BasePFScheduler::collect_relevant_packets(double current_time)
{
    while (main_queue.size() > 0)
    {
        Packet packet = main_queue.front();

        // Если пакет не пришел, его время больше текущего
        if (current_time + epsilon < packet.get_scheduled_at())
        {
            // Конец перебора при встрече первого непришедшего пакета
            break;
        }

        // Если пакет уже был доступен по времени

        // Отметка пользователя как активного претендента на ресурсы
        mark_as_resource_candidate(
            packet.get_queue(), 
            packet.get_user_ptr()
        );

        packet.get_user_ptr()->set_resource_candidate(true);

        sorted_resource_candidates_for_tti.push_back(packet.get_user_ptr());

        // Удаление первого элемента для доступа к следующим
        main_queue.pop();
        // Перенос в промежуточную очередь
        relevant_queue.push(packet);
    }
}

void BasePFScheduler::sort_users()
{
    // Фильтрация неуникальных указателей на пользователей
    std::unordered_set<User *> unique_users(
        sorted_resource_candidates_for_tti.begin(),
        sorted_resource_candidates_for_tti.end());

    sorted_resource_candidates_for_tti.assign(unique_users.begin(), unique_users.end());

    std::sort(
        sorted_resource_candidates_for_tti.begin(),
        sorted_resource_candidates_for_tti.end(),
        UserPFComparator());
}

/*
Подсчет статистики по результатам работы планировщика
*/
void BasePFScheduler::evaluate_stats()
{
    stats.scheduler_total_time =
        session.get_scheduling_end_time() - session.get_scheduling_start_time();
    stats.packet_count = session.get_processed_packet_count();

    double queue_total_time = stats.get_queue_processing_time(0) +
                              stats.get_queue_idle_time(0) +
                              stats.get_queue_wait_time(0);

    stats.set_queue_total_time(0, queue_total_time);

    evaluate_fairness_stats(SchedulerState::UNDEFINED, true);
    evaluate_throughput_stats(true);
}

void BasePFScheduler::sync_user_channels()
{
    for (auto &user_info : connected_users)
    {
        User &user = user_info.second;
        double time_from_last_channel_sync = user.get_time_from_last_channel_sync();
        double new_time_from_last_channel_sync = time_from_last_channel_sync + tti_duration;

        // Если время без сихронизации превышает интервал синхронизации, то сброс таймера в 0
        if (new_time_from_last_channel_sync > channel_sync_interval + epsilon)
        {
            new_time_from_last_channel_sync = 0;
        }

        // Периодеческая синхронизация позиции пользователя
        if (time_from_last_channel_sync <= epsilon)
        {
            user.move(channel_sync_interval);

            double user_to_bs_distance =
                user.get_position().get_distance_2d(
                    base_station.get_position()) /
                1000; // (метры -> км)

            double user_height = user.get_position().get_z();
            double bs_height = base_station.get_position().get_z();

            double path_loss =
                channel.get_path_loss(user_to_bs_distance, bs_height, user_height);

            double user_received_signal_power =
                channel.get_received_signal_power(path_loss);

            double noise_power = channel.get_noise_power();

            double interference_power =
                channel.get_interference_power();

            double sinr = channel.get_sinr(
                user_received_signal_power,
                noise_power,
                interference_power
            );

            int cqi = StandardManager::get_cqi_from_sinr(sinr);

            user.set_cqi(cqi);
        }
        else
        {
            user.set_time_from_last_channel_sync(new_time_from_last_channel_sync);
        }
    }
}

void BasePFScheduler::flush_user_context()
{
    for (auto &user_info : connected_users)
    {
        user_info.second.set_current_throughput(0);
        user_info.second.set_resource_candidate(false);
    }
}

void BasePFScheduler::update_user_priorities()
{
    for (auto &user_info : connected_users)
    {
        // PF-metric = r_i / R_i
        // r_i (bytes per RB)
        int max_throughput_for_rb =
            StandardManager::get_resource_block_effective_data_size(
                user_info.second.get_cqi());

        // R_i (bytes/ms)
        double average_throughput = user_info.second.get_average_throughput();

        double priority = max_throughput_for_rb / average_throughput;

        user_info.second.set_priority(priority);

    }
}

void BasePFScheduler::update_user_throughputs()
{
    for (auto &user_info : connected_users)
    {
        if (user_info.second.is_resource_candidate())
        {
            user_info.second.update_throughput_history();
        }
    }
}

void BasePFScheduler::exclude_users_from_scheduling()
{
    if (sorted_resource_candidates_for_tti.size() <= (size_t)users_per_tti_limit)
    {
        return;
    }

    int priority_collision_start_idx = 0;
    int priority_collision_end_idx = 0;

    for (size_t i = 1; i < sorted_resource_candidates_for_tti.size(); ++i)
    {

        double priority_delta =
            std::fabs(
                sorted_resource_candidates_for_tti[i - 1]->get_priority() -
                sorted_resource_candidates_for_tti[i]->get_priority());

        double avg_throughput_delta =
            std::fabs(
                sorted_resource_candidates_for_tti[i - 1]->get_average_throughput() -
                sorted_resource_candidates_for_tti[i]->get_average_throughput());

        if (priority_delta < epsilon && avg_throughput_delta < epsilon)
        {
            priority_collision_end_idx = i;
        }
        else
        {
            if (i < (size_t)users_per_tti_limit)
            {
                priority_collision_start_idx = i;
                priority_collision_end_idx = i;
            }
            else
            {
                break;
            }
        }
    }

    if (priority_collision_start_idx < users_per_tti_limit)
    {
        // Перемешивание пользователей в диапазоне коллизий
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(
            sorted_resource_candidates_for_tti.begin() + priority_collision_start_idx,
            sorted_resource_candidates_for_tti.begin() + priority_collision_end_idx + 1,
            g);
    }

    // Удаление всех пользователей за пределами `users_per_tti_limit`
    sorted_resource_candidates_for_tti.erase(
        sorted_resource_candidates_for_tti.begin() + users_per_tti_limit,
        sorted_resource_candidates_for_tti.end());
}

void BasePFScheduler::filter_packets_of_excluded_from_scheduling_users()
{
    RelevantPacketQueue tmp_queue;

    while (relevant_queue.size() > 0)
    {
        Packet packet = relevant_queue.front();
        User *user_ptr = packet.get_user_ptr();

        auto it = std::find(
            sorted_resource_candidates_for_tti.begin(),
            sorted_resource_candidates_for_tti.end(),
            user_ptr);

        // Если пакет принадлежит разрешенному пользователю
        if (it != sorted_resource_candidates_for_tti.end())
        {
            tmp_queue.push(packet);
        }
        else
        {
            main_queue.push(packet);
        }

        relevant_queue.pop();
    }

    relevant_queue = std::move(tmp_queue);
}

void BasePFScheduler::reset_served_users()
{
    sorted_resource_candidates_for_tti.clear();
}

/*
Подсчет статистики за TTI по результатам работы планировщика
*/
void BasePFScheduler::evaluate_fairness_stats(
    SchedulerState scheduler_state, bool force_update)
{
    if (force_update)
    {
        fairness_stats.calculate_fairness_for_queues();
        fairness_stats.calculate_fairness_for_users();

        stats.update_scheduler_fairness_for_queues(
            fairness_stats.get_current_history_size(),
            fairness_stats.get_fairness_for_queues(),
            fairness_stats.is_valid_fairness_for_queues());

        stats.update_scheduler_fairness_for_users(
            fairness_stats.get_current_history_size(),
            fairness_stats.get_fairness_for_users(),
            fairness_stats.is_valid_fairness_for_users());

        fairness_stats.reset();

        return;
    }

    if (scheduler_state > SchedulerState::WAIT){
        fairness_stats.increment_current_history_size();
    }

    if (fairness_stats.is_history_size_limit_reached())
    {
        fairness_stats.calculate_fairness_for_queues();
        fairness_stats.calculate_fairness_for_users();

        stats.update_scheduler_fairness_for_queues(
            fairness_stats.get_current_history_size(),
            fairness_stats.get_fairness_for_queues(),
            fairness_stats.is_valid_fairness_for_queues());

        stats.update_scheduler_fairness_for_users(
            fairness_stats.get_current_history_size(),
            fairness_stats.get_fairness_for_users(),
            fairness_stats.is_valid_fairness_for_users());

        fairness_stats.reset();
    }
}

/*
Подсчет статистики за TTI по результатам работы планировщика
*/
void BasePFScheduler::evaluate_throughput_stats(bool force_update)
{
    if (force_update)
    {
        throughput_stats.calculate_throughput_for_scheduler();
        stats.update_scheduler_throughput(
            throughput_stats.get_throughput_for_scheduler(),
            throughput_stats.is_valid_throughput_for_scheduler());

        throughput_stats.calculate_unused_resources_for_scheduler();
        stats.update_scheduler_unused_resources(
            throughput_stats.get_unused_resources_for_scheduler(),
            throughput_stats.is_valid_unused_resources_for_scheduler());

        throughput_stats.reset();

        return;
    }

    throughput_stats.calculate_throughput_for_scheduler();
    stats.update_scheduler_throughput(
        throughput_stats.get_throughput_for_scheduler(),
        throughput_stats.is_valid_throughput_for_scheduler());

    throughput_stats.calculate_unused_resources_for_scheduler();
    stats.update_scheduler_unused_resources(
        throughput_stats.get_unused_resources_for_scheduler(),
        throughput_stats.is_valid_unused_resources_for_scheduler());

    throughput_stats.reset();
}