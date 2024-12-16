#include <vector>
#include <algorithm>
#include "settings.hpp"


#define RB_BANDWIDTH 0.180 // Ширина RB в МГц

std::vector<double> allowed_bandwidths = { 1.4, 3, 5, 10, 15, 20 };

Settings::Settings(
    int launches, double bandwidth, 
    int packet_count, int packet_size, 
    int queue_count, int queue_quant, int queue_limit,
    double time_lambda)
    {
        this->launches = launches;
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

    auto is_allowed_bandwidth = 
        std::find(
            allowed_bandwidths.begin(), 
            allowed_bandwidths.end(), 
            bandwidth
        ) != allowed_bandwidths.end();

    if (bandwidth < 1 || !is_allowed_bandwidth) {
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