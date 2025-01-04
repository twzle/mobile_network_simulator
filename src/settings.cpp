#include "settings.hpp"

#define RB_BANDWIDTH 0.180 // Ширина RB в МГц

Settings::Settings(
    int launches, 
    std::string standard_type, std::string tti_duration,
    std::string scheduler_type,
    double bandwidth, 
    int packet_count, int packet_size, 
    int queue_count, int queue_quant, int queue_limit,
    double time_lambda)
    {
        this->launches = launches;
        this->standard_type = standard_type;
        this->tti_duration = tti_duration;
        this->scheduler_type = scheduler_type;
        this->bandwidth = bandwidth;
        this->packet_count = packet_count;
        this->packet_size = packet_size;
        this->queue_count = queue_count;
        this->queue_quant = queue_quant;
        this->queue_limit = queue_limit;
        this->time_lambda = time_lambda;
    }


void Settings::validate() {
    if (launches < 1) {
        throw std::invalid_argument("Launches should be greater than or equal to 1.");
    }

    StandardInfo standard_info;
    try {
        standard_info = StandardManager::get_standard_info(standard_type);
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid standard.");
    }

    try {
        StandardManager::get_tti(tti_duration);
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid TTI.");
    }

    auto is_allowed_scheduler_type = 
        std::find(
            standard_info.schedulers.begin(), 
            standard_info.schedulers.end(), 
            scheduler_type
        ) != standard_info.schedulers.end();

    if (!is_allowed_scheduler_type) {
        throw std::invalid_argument("Invalid scheduler type.");
    }

    auto is_allowed_bandwidth = 
        std::find(
            standard_info.bandwidths.begin(), 
            standard_info.bandwidths.end(), 
            bandwidth
        ) != standard_info.bandwidths.end();

    if (!is_allowed_bandwidth) {
        throw std::invalid_argument("Invalid bandwidth.");
    }

    if (packet_count < 1) {
        throw std::invalid_argument("Packet count should be greater than or equal to 1.");
    }

    if (packet_size < 1) {
        throw std::invalid_argument("Packet size should be greater than or equal to 1.");
    }

    if (packet_size > get_resource_block_per_tti_limit()) {
        throw std::invalid_argument("Packet size exceeds resource block limit.");
    }

    if (queue_count < 1) {
        throw std::invalid_argument("Queue count should be greater than or equal to 1.");
    }

    if (queue_quant < 1) {
        throw std::invalid_argument("Queue quant should be greater than or equal to 1.");
    }

    if (time_lambda < 1) {
        throw std::invalid_argument("Time lambda should be greater than or equal to 1.");
    }
}


int Settings::get_launches(){
    return this->launches;
}

std::string Settings::get_standard_type(){
    return this->standard_type;
}

std::string Settings::get_tti_duration(){
    return this->tti_duration;
}

std::unique_ptr<BaseDRRScheduler> Settings::get_scheduler_instance(){
    double tti = StandardManager::get_tti(tti_duration);

    if (this->scheduler_type == "FixedDRRScheduler") {
        return std::make_unique<FixedDRRScheduler>(tti);
    } else if (this->scheduler_type == "CircularDRRScheduler") {
        return std::make_unique<CircularDRRScheduler>(tti);
    } else if (this->scheduler_type == "DefaultDRRScheduler") {
        return std::make_unique<DefaultDRRScheduler>(tti);
    }
    return nullptr;
}

double Settings::get_bandwidth(){
    return this->bandwidth;
}

int Settings::get_packet_count(){
    return this->packet_count;
}

int Settings::get_packet_size(){
    return this->packet_size;
}

int Settings::get_queue_count(){
    return this->queue_count;
}

int Settings::get_queue_quant(){
    return this->queue_quant;
}

int Settings::get_queue_limit(){
    return this->queue_limit;
}

double Settings::get_time_lambda(){
    return this->time_lambda;
}

int Settings::get_resource_block_per_tti_limit(){
    int resource_block_per_tti_limit = this->bandwidth/RB_BANDWIDTH;
    return resource_block_per_tti_limit;
}