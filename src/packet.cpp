#include "packet.hpp"

Packet::Packet(int size = 0) : size(size){}

ms Packet::get_size() { return size; }
bool Packet::get_retry() { return retry; }
void Packet::set_retry() { this->retry = true; }