#include "core/packet.hpp"

Packet::Packet(int queue, int size, User *user)
    : size(size), scheduled_at(0.0), queue(queue), user(user) {}

int Packet::get_size() const { return size; }
void Packet::set_size(int size) { this->size = size; }

int Packet::get_queue() const { return queue; }
void Packet::set_queue(int queue) { this->queue = queue; }

User *Packet::get_user_ptr() const { return user; }
void Packet::set_user_ptr(User *user) { this->user = user; }

double Packet::get_scheduled_at() const { return scheduled_at; }
void Packet::set_scheduled_at(double scheduled_at)
{
    this->scheduled_at = scheduled_at;
}