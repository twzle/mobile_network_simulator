#include "config/queue_config.hpp"

QueueConfig::QueueConfig(int packet_count, double quant) : 
    packet_count(packet_count), quant(quant)
{
}

int QueueConfig::get_packet_count()
{
    return this->packet_count;
}

double QueueConfig::get_quant()
{
    return this->quant;
}