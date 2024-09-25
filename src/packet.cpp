#include "packet.hpp"

Packet::Packet(int size = 0) : size(size), retry(false){
    scheduled_at = SystemClock::now();
}

ms Packet::get_size() { return size; }
bool Packet::get_retry() { return retry; }
void Packet::set_retry() { this->retry = true; }

time_point Packet::get_scheduled_at() { return scheduled_at; }
void Packet::set_scheduled_at(time_point scheduled_at) { this->scheduled_at = scheduled_at; }