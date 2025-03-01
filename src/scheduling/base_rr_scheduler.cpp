#include "scheduling/base_rr_scheduler.hpp"

BaseRRScheduler::BaseRRScheduler(
    double tti,
    double channel_sync_interval,
    uint8_t base_cqi,
    Channel channel)
    : tti_duration(tti),
      channel_sync_interval(channel_sync_interval),
      base_cqi(base_cqi),
      channel(channel) {}

/*
Планирование очереди через запись в массив очередей
и вычисление новой суммы общего количества пакетов
*/
void BaseRRScheduler::schedule(PacketQueue &&packet_queue)
{
    scheduled_queues.push_back(packet_queue);
    total_packets += packet_queue.size();
}

void BaseRRScheduler::configure_base_station(BSConfig bs_config)
{
    Position position = Position(
        bs_config.get_x(),
        bs_config.get_y(),
        bs_config.get_z());

    base_station = BaseStation(position);
}

void BaseRRScheduler::connect_users(std::vector<UserConfig> user_configs)
{
    for (size_t i = 0; i < user_configs.size(); ++i)
    {
        UserConfig user_cfg = user_configs[i];

        Position position = Position(
            user_cfg.get_x(),
            user_cfg.get_y(),
            user_cfg.get_z());

        Mobility mobility = Mobility(
            user_cfg.get_speed(),
            user_cfg.get_direction());

        User user(base_cqi, position, mobility);
        connected_users.emplace(user.get_id(), std::move(user));
    }
}

User *BaseRRScheduler::get_user_ptr(int user_id)
{
    auto it = connected_users.find(user_id);
    if (it != connected_users.end())
    {
        // Получаем указатель на пользователя
        User *user_ptr = &it->second;
        return user_ptr;
    }

    return nullptr;
}

void BaseRRScheduler::set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit)
{
    this->resource_blocks_per_tti = resource_blocks_per_tti_limit;
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
    PacketQueue &queue, double current_time, TTIStats &tti_stats)
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
        if (packet.get_scheduled_at() < current_time - epsilon)
        {
            // Первый непришедший пакет учтен DRR, остальные нет
            if (packet_count > 0)
            {
                // Отметка пользователя как активного претендента на ресурсы
                tti_stats.mark_user_as_resource_candidate(
                    packet.get_user_ptr());
            }
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

// Перевод размера пакета из байтов в ресурсные блоки согласно размеру полезных данных в одном RB
int BaseRRScheduler::convert_packet_size_to_rb_number(
    User *user, int packet_size)
{
    double effective_data_size_per_rb_for_user_in_bytes =
        (StandardManager::get_cqi_efficiency(user->get_cqi()) *
         StandardManager::get_resource_elements_in_resource_block()) /
        8;

    int rb_count =
        static_cast<int>(
            std::ceil(
                static_cast<double>(packet_size) /
                effective_data_size_per_rb_for_user_in_bytes));

    return rb_count;
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
            std::cout << "User #" << user.get_id() << ". " << user.get_position() << std::endl;
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
                channel.get_interference_power(user_received_signal_power);

            double sinr = channel.get_sinr(
                user_received_signal_power,
                noise_power, interference_power);
            
            int cqi = StandardManager::get_cqi_from_sinr(sinr);
            std::cout << cqi << "\n";

            user.set_cqi(base_cqi);
        }
        else
        {
            user.set_time_from_last_channel_sync(new_time_from_last_channel_sync);
        }
    }

    check_start_pos = false;
}

IterationStats &BaseRRScheduler::get_stats()
{
    return this->stats;
}