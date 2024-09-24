#include "packet.hpp"

Packet::Packet(int size = 0) : size(size){}

ms Packet::get_size() { return size; }