#include "packet.hpp"

Packet::Packet(int size = 0) : Packet(size, SystemClock::now()) {}
Packet::Packet(int size, TimePoint created_at) : size(size), created_at(created_at) {}

ms Packet::get_size() { return size; }
TimePoint Packet::get_time() { return created_at; }