#include "core/packet.hpp"

Packet::Packet(int size = 0) : size(size), scheduled_at(0.0), retry(false) {}

int Packet::get_size() const { return size; }
bool Packet::get_retry() const { return retry; }
void Packet::set_retry() { this->retry = true; }

double Packet::get_scheduled_at() const { return scheduled_at; }
void Packet::set_scheduled_at(double scheduled_at)
{
    this->scheduled_at = scheduled_at;
}