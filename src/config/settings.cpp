#include "config/settings.hpp"
#include <sstream>

Settings::Settings(
    int launches,
    std::string standard_type,
    uint8_t base_cqi,
    std::string tti_duration,
    std::string channel_sync_interval,
    std::string scheduler_type,
    double bandwidth,
    int packet_count, int packet_size,
    int queue_count, double queue_quant, int queue_limit,
    double time_lambda,
    std::vector<UserConfig> user_configs,
    BSConfig bs_config,
    double carrier_frequency,
    int bs_transmission_power,
    std::string area_type,
    int users_per_tti_limit,
    double throughput_history_size)
{
    this->launches = launches;
    this->standard_type = standard_type;
    this->base_cqi = base_cqi;
    this->tti_duration = tti_duration;
    this->channel_sync_interval = channel_sync_interval;
    this->scheduler_type = scheduler_type;
    this->bandwidth = bandwidth;
    this->packet_count = packet_count;
    this->packet_size = packet_size;
    this->queue_count = queue_count;
    this->queue_quant = queue_quant;
    this->queue_limit = queue_limit;
    this->time_lambda = time_lambda;
    this->user_configs = user_configs;
    this->user_count = user_configs.size();
    this->bs_config = bs_config;
    this->carrier_frequency = carrier_frequency;
    this->bs_transmission_power = bs_transmission_power;
    this->area_type = area_type;
    this->users_per_tti_limit = users_per_tti_limit;
    this->throughput_history_size = throughput_history_size;
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
    return this->bandwidth;
}

int Settings::get_packet_count()
{
    return this->packet_count;
}

int Settings::get_packet_size()
{
    return this->packet_size;
}

int Settings::get_queue_count()
{
    return this->queue_count;
}

int Settings::get_user_count()
{
    return this->user_count;
}

double Settings::get_queue_quant()
{
    return this->queue_quant;
}

int Settings::get_queue_limit()
{
    return this->queue_limit;
}

BSConfig Settings::get_bs_config()
{
    return this->bs_config;
}

std::vector<UserConfig> Settings::get_user_configs()
{
    return this->user_configs;
}

double Settings::get_time_lambda()
{
    return this->time_lambda;
}

double Settings::get_carrier_frequency()
{
    return this->carrier_frequency;
}

int Settings::get_bs_transmission_power()
{
    return this->bs_transmission_power;
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
        StandardManager::get_rb_number_from_bandwidth(bandwidth);

    return resource_block_per_tti_limit;
}

int Settings::get_packet_size_limit()
{
    int rb_per_tti = get_resource_block_per_tti_limit();

    int bytes_per_rb =
        StandardManager::get_resource_block_effective_data_size(
            base_cqi
        );

    double bytes_per_tti = rb_per_tti * bytes_per_rb;

    return bytes_per_tti;
}

int Settings::get_throughput_history_size()
{
    return this->throughput_history_size;
}
