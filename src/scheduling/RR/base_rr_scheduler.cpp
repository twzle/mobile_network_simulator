#include "scheduling/RR/base_rr_scheduler.hpp"

BaseRRScheduler::BaseRRScheduler() : BaseScheduler() {}

/*
Планирование очереди через запись в массив очередей
и вычисление новой суммы общего количества пакетов
*/
void BaseRRScheduler::schedule(PacketQueue &&packet_queue)
{
    scheduled_queues.push_back(packet_queue);
    total_packets += packet_queue.size();
}

void BaseRRScheduler::set_initial_queue(size_t new_initial_queue_id)
{
    this->current_initial_absolute_queue_id = new_initial_queue_id;
}

size_t BaseRRScheduler::get_initial_queue()
{
    return this->current_initial_absolute_queue_id;
}

size_t BaseRRScheduler::get_relative_queue_id(size_t current_absolute_queue_id)
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

void BaseRRScheduler::check_queue_remaining_scheduled_packets(
    PacketQueue &queue, double current_time)
{
    PacketQueue tmp(queue.get_quant(), queue.get_limit());

    int packet_count = 0;

    // Проверка пакетов исходной очереди на факт доступности
    while (queue.size() > 0)
    {
        Packet packet = queue.front();

        // Если пакет не пришел, его время больше текущего
        if (current_time + epsilon < packet.get_scheduled_at())
        {
            // Конец перебора при встрече первого непришедшего пакета
            break;
        }

        // Если пакет уже был доступен по времени

        // Первый необслуженный пакет учтен DRR, остальные нет
        if (packet_count > 0)
        {
            mark_as_resource_candidate(
                packet.get_queue(), 
                packet.get_user_ptr()
            );
        }

        // Удаление первого элемента для доступа к следующим
        queue.pop();
        // Сохранение во временной очереди пакета для восстановления
        tmp.push(packet);

        ++packet_count;
    }

    // Восстановление пакетов в исходной очереди из временной
    while (tmp.size() > 0)
    {
        Packet packet = tmp.front();

        // Удаление первого элемента для доступа к следующим
        tmp.pop();
        // Восстановление пакета из временной очереди
        queue.push(packet);
    }
}

/*
Подсчет статистики по результатам работы планировщика
*/
void BaseRRScheduler::evaluate_stats()
{
    stats.scheduler_total_time =
        session.get_scheduling_end_time() - session.get_scheduling_start_time();
    stats.packet_count = session.get_processed_packet_count();

    for (size_t queue_id = 0; queue_id < scheduled_queues.size(); ++queue_id)
    {
        double queue_total_time = stats.get_queue_processing_time(queue_id) +
                                  stats.get_queue_idle_time(queue_id) +
                                  stats.get_queue_wait_time(queue_id);

        stats.set_queue_total_time(queue_id, queue_total_time);
    }
}

/*
Подсчет статистики за TTI по результатам работы планировщика
*/
void BaseRRScheduler::evaluate_fairness_stats()
{
    if (fairness_stats.is_history_size_limit_reached())
    {
        fairness_stats.calculate_fairness_for_queues();
        stats.update_scheduler_fairness_for_queues(
            fairness_stats.get_fairness_for_queues(),
            fairness_stats.is_valid_fairness_for_queues());

        fairness_stats.calculate_fairness_for_users();
        stats.update_scheduler_fairness_for_users(
            fairness_stats.get_fairness_for_users(),
            fairness_stats.is_valid_fairness_for_users());

        if (fairness_stats.is_valid_fairness_for_queues() ||
            fairness_stats.is_valid_fairness_for_users())
        {
            fairness_stats.increment_current_history_size();
        }

        fairness_stats.reset();
    }
}

/*
Подсчет статистики за TTI по результатам работы планировщика
*/
void BaseRRScheduler::evaluate_throughput_stats()
{
    throughput_stats.calculate_throughput_for_scheduler();
    stats.update_scheduler_throughput(
        throughput_stats.get_throughput_for_scheduler(),
        throughput_stats.is_valid_throughput_for_scheduler());
    throughput_stats.reset();
}

void BaseRRScheduler::sync_user_channels()
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
            // std::cout << "User #" << user.get_id() << ". " << user.get_position() << std::endl;
            user.move(channel_sync_interval);

            double user_to_bs_distance = user.get_position().get_distance_2d(
                base_station.get_position());
            double user_height = user.get_position().get_z();
            double bs_height = base_station.get_position().get_z();

            double path_loss =
                channel.get_path_loss(user_to_bs_distance, bs_height, user_height);

            double user_received_signal_power =
                channel.get_recieved_signal_power(path_loss);
            double noise_power = channel.get_noise_power();
            double interference_power =
                channel.get_interference_power();

            double sinr = channel.get_sinr(
                user_received_signal_power,
                noise_power, interference_power);

            StandardManager::get_cqi_from_sinr(sinr);
            // std::cout << cqi << "\n";

            user.set_cqi(base_cqi);
        }
        else
        {
            user.set_time_from_last_channel_sync(new_time_from_last_channel_sync);
        }
    }
}

void BaseRRScheduler::reset_served_users()
{
    users_served_in_tti.clear();
}