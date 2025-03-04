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

void BaseScheduler::set_users(std::vector<UserConfig> user_configs)
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

        User user(base_cqi, position, mobility, 0);
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

void BaseScheduler::set_channel(Channel channel){
    this->channel = channel;
}

IterationStats &BaseScheduler::get_stats()
{
    return this->stats;
}

// Перевод размера пакета из байтов в ресурсные блоки согласно размеру полезных данных в одном RB
int BaseScheduler::convert_packet_size_to_rb_number(
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