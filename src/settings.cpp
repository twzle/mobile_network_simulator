#include "settings.hpp"
#include <sstream>

#define RB_BANDWIDTH 0.180 // Ширина RB в МГц

Settings::Settings(
    int launches,
    std::string standard_type,
    uint8_t cqi,
    std::string tti_duration,
    std::string channel_sync_interval,
    std::string scheduler_type,
    double bandwidth,
    int packet_count, int packet_size,
    int queue_count, double queue_quant, int queue_limit,
    int user_count,
    double time_lambda)
{
    this->launches = launches;
    this->standard_type = standard_type;
    this->cqi = cqi;
    this->tti_duration = tti_duration;
    this->channel_sync_interval = channel_sync_interval;
    this->scheduler_type = scheduler_type;
    this->bandwidth = bandwidth;
    this->packet_count = packet_count;
    this->packet_size = packet_size;
    this->queue_count = queue_count;
    this->queue_quant = queue_quant;
    this->queue_limit = queue_limit;
    this->user_count = user_count;
    this->time_lambda = time_lambda;
}

void Settings::validate()
{
    if (launches < 1)
    {
        throw std::invalid_argument("Launches should be greater than or equal to 1.");
    }

    StandardInfo standard_info;
    try
    {
        standard_info = StandardManager::get_standard_info(standard_type);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid standard.");
    }

    try
    {
        StandardManager::get_tti(standard_type, tti_duration);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid TTI.");
    }

    try
    {
        StandardManager::get_channel_sync_interval(standard_type, channel_sync_interval);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid channel sync interval.");
    }

    try
    {
        StandardManager::get_cqi_efficiency(standard_type, cqi);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid CQI.");
    }

    auto is_allowed_scheduler_type =
        std::find(
            standard_info.schedulers.begin(),
            standard_info.schedulers.end(),
            scheduler_type) != standard_info.schedulers.end();

    if (!is_allowed_scheduler_type)
    {
        throw std::invalid_argument("Invalid scheduler type.");
    }

    auto is_allowed_bandwidth =
        std::find(
            standard_info.bandwidths.begin(),
            standard_info.bandwidths.end(),
            bandwidth) != standard_info.bandwidths.end();

    if (!is_allowed_bandwidth)
    {
        throw std::invalid_argument("Invalid bandwidth.");
    }

    if (packet_count < 1)
    {
        throw std::invalid_argument("Packet count should be greater than or equal to 1.");
    }

    if (packet_size < 1 || packet_size > get_packet_size_limit())
    {
        std::ostringstream msg;
        msg << "Invalid packet size (current range [1, "
            << get_packet_size_limit() << "] bytes)."
            << "Maximum defined by current bandwidth.";
        throw std::invalid_argument(msg.str());
    }

    if (queue_count < 1)
    {
        throw std::invalid_argument("Queue count should be greater than or equal to 1.");
    }

    if (queue_quant + epsilon < 0)
    {
        throw std::invalid_argument("Queue quant should be greater than or equal to 0.");
    }

    if (time_lambda < 1)
    {
        throw std::invalid_argument("Time lambda should be greater than or equal to 1.");
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

uint8_t Settings::get_cqi()
{
    return this->cqi;
}

std::string Settings::get_tti_duration()
{
    return this->tti_duration;
}

std::string Settings::get_channel_sync_interval()
{
    return this->channel_sync_interval;
}

std::unique_ptr<BaseRRScheduler> Settings::get_scheduler_instance()
{
    double tti_value = 
        StandardManager::get_tti(standard_type, tti_duration);
    double channel_sync_interval_value =
        StandardManager::get_channel_sync_interval(standard_type, channel_sync_interval);

    if (this->scheduler_type == "DefaultRRScheduler")
    {
        return std::make_unique<DefaultRRScheduler>(
            standard_type, tti_value, channel_sync_interval_value, cqi);
    }
    else if (this->scheduler_type == "FixedDRRScheduler")
    {
        return std::make_unique<FixedDRRScheduler>(
            standard_type, tti_value, channel_sync_interval_value, cqi);
    }
    else if (this->scheduler_type == "CyclicDRRScheduler")
    {
        return std::make_unique<CyclicDRRScheduler>(
            standard_type, tti_value, channel_sync_interval_value, cqi);
    }
    else if (this->scheduler_type == "DefaultDRRScheduler")
    {
        return std::make_unique<DefaultDRRScheduler>(
            standard_type, tti_value, channel_sync_interval_value, cqi);
    }
    return nullptr;
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

double Settings::get_queue_quant()
{
    return this->queue_quant;
}

int Settings::get_queue_limit()
{
    return this->queue_limit;
}

int Settings::get_user_count()
{
    return this->user_count;
}

double Settings::get_time_lambda()
{
    return this->time_lambda;
}

int Settings::get_resource_block_per_tti_limit()
{
    int resource_block_per_tti_limit = this->bandwidth / RB_BANDWIDTH;
    return resource_block_per_tti_limit;
}

int Settings::get_packet_size_limit()
{
    int rb_per_tti_limit = get_resource_block_per_tti_limit();

    double bit_per_re = StandardManager::get_cqi_efficiency(
        standard_type, cqi);
    uint8_t re_per_rb = StandardManager::get_resource_elements_in_resource_block(
        standard_type);

    double bit_per_tti_limit = rb_per_tti_limit * bit_per_re * re_per_rb;
    int byte_per_tti_limit = bit_per_tti_limit / 8;

    return byte_per_tti_limit;
}
