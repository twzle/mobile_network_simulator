#include "config/queue_config.hpp"

QueueConfig::QueueConfig(
    int packet_count, double quant, double time_lambda) : 
    packet_count(packet_count), quant(quant), time_lambda(time_lambda)
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

double QueueConfig::get_time_lambda()
{
    return this->time_lambda;
}