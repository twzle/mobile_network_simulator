#include "scheduling/base_scheduler.hpp"

BaseScheduler::BaseScheduler() {}

void BaseScheduler::set_base_station(BSConfig bs_config)
{
    Position position = Position(
        bs_config.get_x(),
        bs_config.get_y(),
        bs_config.get_z());

    base_station = BaseStation(position);
}

void BaseScheduler::set_users(
    std::vector<UserConfig> user_configs, int throughput_history_size)
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

        double quant = user_cfg.get_quant();

        User user(base_cqi, position, mobility, throughput_history_size, quant);
        connected_users.emplace(user.get_id(), std::move(user));
    }
}

User *BaseScheduler::get_user_ptr(int user_id)
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

void BaseScheduler::set_tti_duration(double tti_duration)
{
    this->tti_duration = tti_duration;
}

void BaseScheduler::set_resource_block_per_tti_limit(int resource_blocks_per_tti_limit)
{
    this->resource_blocks_per_tti = resource_blocks_per_tti_limit;
}

void BaseScheduler::set_channel_sync_interval(double channel_sync_interval)
{
    this->channel_sync_interval = channel_sync_interval;
}

void BaseScheduler::set_base_cqi(uint8_t base_cqi)
{
    this->base_cqi = base_cqi;
}

void BaseScheduler::set_channel(Channel channel)
{
    this->channel = channel;
}

void BaseScheduler::set_users_per_tti_limit(int users_per_tti_limit)
{
    this->users_per_tti_limit = users_per_tti_limit;
}

void BaseScheduler::set_tti_stats(
    int resource_block_limit_per_tti,
    size_t queue_count, size_t user_count,
    double tti_duration, int history_size_limit)
{
    fairness_stats.initialize(
        queue_count, user_count,
        tti_duration, history_size_limit);

    throughput_stats.initialize(
        resource_block_limit_per_tti,
        queue_count, user_count,
        tti_duration, history_size_limit);
}

IterationStats &BaseScheduler::get_stats()
{
    return this->stats;
}

// Перевод размера пакета из байтов в ресурсные блоки согласно размеру полезных данных в одном RB
int BaseScheduler::convert_packet_size_to_rb_number(
    User *user, int packet_size)
{
    std::tuple<std::string, double, double, int> mcs =
        StandardManager::get_mcs_from_cqi(
            user->get_cqi());
    
    int imcs = std::get<3>(mcs);
    int itbs = StandardManager::get_tbs_from_mcs(imcs);

    int rb_count = TBS::find_min_rb_for_packet(itbs, packet_size);

    return rb_count;
}

void BaseScheduler::save_processed_packet_stats(
    Packet &packet, int packet_size_in_bytes, int packet_size_in_rb, 
    double current_time)
{
    fairness_stats.add_allocated_effective_data_to_queue(
        packet.get_queue(),
        packet_size_in_bytes);
    throughput_stats.add_allocated_effective_data_to_queue(
        packet.get_queue(),
        packet_size_in_bytes);

    fairness_stats.add_allocated_effective_data_to_user(
        packet.get_user_ptr(),
        packet_size_in_bytes);
    throughput_stats.add_allocated_effective_data_to_user(
        packet.get_user_ptr(),
        packet_size_in_bytes);

    fairness_stats.add_allocated_effective_data_to_total(
        packet_size_in_bytes);
    throughput_stats.add_allocated_effective_data_to_total(
        packet_size_in_bytes, packet_size_in_rb);

    stats.add_queue_packet_stats(
        packet.get_queue(),
        packet.get_user_ptr()->get_id(),
        current_time - packet.get_scheduled_at());
}

void BaseScheduler::mark_as_resource_candidate(int queue, User *user)
{
    fairness_stats.mark_user_as_resource_candidate(user);
    throughput_stats.mark_user_as_resource_candidate(user);
    
    fairness_stats.mark_queue_as_resource_candidate(queue);
    throughput_stats.mark_queue_as_resource_candidate(queue);
}