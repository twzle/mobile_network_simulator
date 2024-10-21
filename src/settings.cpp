#include "settings.hpp"

Settings::Settings(
    int launches, int packet_count, int packet_size, 
    int queue_count, int queue_quant, int queue_limit)
    {
        this->launches = launches;
        this->packet_count = packet_count;
        this->packet_size = packet_size;
        this->queue_count = queue_count;
        this->queue_quant = queue_quant;
        this->queue_limit = queue_limit;
    }

int Settings::get_launches(){
    return this->launches;
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