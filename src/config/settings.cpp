#include "config/settings.hpp"
#include <sstream>

Settings::Settings() {
    this->launches = 1;
    this->standard_type = "LTE";
    this->base_cqi = 1;
    this->tti_duration = "1ms";
    this->channel_sync_interval = "10ms";
    this->scheduler_type = "DefaultDRRScheduler";
    this->queue_count = queue_configs.size();
    this->user_count = user_configs.size();
    this->bs_config = BSConfig();
    this->area_type = "urban";
    this->users_per_tti_limit = 4;
    this->throughput_history_size = 200;
    this->fairness_history_size = 1;
    this->total_packet_count = 0;

    for (auto& queue_config : queue_configs){
        this->total_packet_count += queue_config.get_packet_count();
    }
}

Settings::Settings(
    int launches,
    std::string standard_type,
    uint8_t base_cqi,
    std::string tti_duration,
    std::string channel_sync_interval,
    std::string scheduler_type,
    std::vector<QueueConfig> queue_configs,
    std::vector<UserConfig> user_configs,
    BSConfig bs_config,
    std::string area_type,
    int users_per_tti_limit,
    int throughput_history_size,
    int fairness_history_size
)
{
    this->launches = launches;
    this->standard_type = standard_type;
    this->base_cqi = base_cqi;
    this->tti_duration = tti_duration;
    this->channel_sync_interval = channel_sync_interval;
    this->scheduler_type = scheduler_type;
    this->queue_configs = queue_configs;
    this->queue_count = queue_configs.size();
    this->user_configs = user_configs;
    this->user_count = user_configs.size();
    this->bs_config = bs_config;
    this->area_type = area_type;
    this->users_per_tti_limit = users_per_tti_limit;
    this->throughput_history_size = throughput_history_size;
    this->fairness_history_size = fairness_history_size;
    this->total_packet_count = 0;

    for (auto& queue_config : queue_configs){
        this->total_packet_count += queue_config.get_packet_count();
    }
}

int Settings::get_launches()
{
    return this->launches;
}

std::string Settings::get_standard_type()
{
    return this->standard_type;
}

uint8_t Settings::get_base_cqi()
{
    return this->base_cqi;
}

double Settings::get_tti_value()
{
    return StandardManager::get_tti(tti_duration);
}

int Settings::get_fairness_history_size()
{
    return this->fairness_history_size;
}

std::string Settings::get_channel_sync_interval()
{
    return this->channel_sync_interval;
}

double Settings::get_channel_sync_interval_value()
{
    return StandardManager::get_channel_sync_interval(channel_sync_interval);
}

std::unique_ptr<BaseScheduler> Settings::get_scheduler_instance()
{
    if (this->scheduler_type == "DefaultRRScheduler")
    {
        return std::make_unique<DefaultRRScheduler>();
    }
    else if (this->scheduler_type == "FixedDRRScheduler")
    {
        return std::make_unique<FixedDRRScheduler>();
    }
    else if (this->scheduler_type == "FixedDRRSchedulerWithUserQuant")
    {
        return std::make_unique<FixedDRRSchedulerWithUserQuant>();
    }
    else if (this->scheduler_type == "CyclicDRRScheduler")
    {
        return std::make_unique<CyclicDRRScheduler>();
    }
    else if (this->scheduler_type == "CyclicDRRSchedulerWithUserQuant")
    {
        return std::make_unique<CyclicDRRSchedulerWithUserQuant>();
    }
    else if (this->scheduler_type == "DefaultDRRScheduler")
    {
        return std::make_unique<DefaultDRRScheduler>();
    }
    else if (this->scheduler_type == "DefaultDRRSchedulerWithUserQuant")
    {
        return std::make_unique<DefaultDRRSchedulerWithUserQuant>();
    }
    else if (this->scheduler_type == "DefaultPFScheduler")
    {
        return std::make_unique<BasePFScheduler>();
    }
    return nullptr;
}

std::string Settings::get_scheduler_name()
{
    return this->scheduler_type;
}

double Settings::get_bandwidth()
{
    return this->bs_config.get_bandwidth();
}

int Settings::get_total_packet_count()
{
    return this->total_packet_count;
}

int Settings::get_queue_count()
{
    return this->queue_count;
}

int Settings::get_user_count()
{
    return this->user_count;
}

BSConfig Settings::get_bs_config()
{
    return this->bs_config;
}

std::vector<QueueConfig> Settings::get_queue_configs()
{
    return this->queue_configs;
}

QueueConfig Settings::get_queue_config(int queue_id)
{
    return this->queue_configs[queue_id];
}

std::vector<UserConfig> Settings::get_user_configs()
{
    return this->user_configs;
}

UserConfig Settings::get_user_config(int user_id)
{
    return this->user_configs[user_id];
}

double Settings::get_carrier_frequency()
{
    return this->bs_config.get_carrier_frequency();
}

int Settings::get_bs_transmission_power()
{
    return this->bs_config.get_transmission_power();
}

std::string Settings::get_area_type()
{
    return this->area_type;
}

int Settings::get_users_per_tti_limit()
{
    return this->users_per_tti_limit;
}

int Settings::get_resource_block_per_tti_limit()
{
    int resource_block_per_tti_limit =
        StandardManager::get_rb_number_from_bandwidth(bs_config.get_bandwidth());

    return resource_block_per_tti_limit;
}

int Settings::get_packet_size_limit()
{
    int rb_per_tti = get_resource_block_per_tti_limit();

    std::tuple<std::string, double, double, int> mcs = 
        StandardManager::get_mcs_from_cqi(base_cqi);
    
    int imcs = std::get<3>(mcs);

    int itbs = StandardManager::get_tbs_from_mcs(imcs);

    int max_bytes_per_rb = TBS::get_size_for_rb(itbs, rb_per_tti);

    return max_bytes_per_rb;
}

int Settings::get_throughput_history_size()
{
    return this->throughput_history_size;
}
