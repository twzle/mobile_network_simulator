#include "scheduling/scheduler_session.hpp"

void SchedulerSession::increment_processed_packet_count(int increment)
{
    this->processed_packets += increment;
}

int SchedulerSession::get_processed_packet_count(){
    return this->processed_packets;
}

void SchedulerSession::set_scheduling_start_time(double time)
{
    this->scheduling_start = time;
}

void SchedulerSession::set_scheduling_end_time(double time)
{
    this->scheduling_end = time;
}

double SchedulerSession::get_scheduling_start_time()
{
    return this->scheduling_start;
}

double SchedulerSession::get_scheduling_end_time()
{
    return this->scheduling_end;
}