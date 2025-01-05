#include "core/packet.hpp"

Packet::Packet(int queue, int size = 0) : size(size), scheduled_at(0.0), queue(queue) {}

int Packet::get_size() const { return size; }
void Packet::set_size(int size){ this->size = size; }

int Packet::get_queue() const { return queue; }
void Packet::set_queue(int queue){ this->queue = queue; }

double Packet::get_scheduled_at() const { return scheduled_at; }
void Packet::set_scheduled_at(double scheduled_at)
{
    this->scheduled_at = scheduled_at;
}