#include "packet_queue.hpp"

PacketQueue::PacketQueue(int quant) : quant(quant), deficit(0){};

void PacketQueue::add_packet(const Packet &packet) { packet_queue.push(packet); }

ms PacketQueue::get_quant() const { return quant; }
ms PacketQueue::get_deficit() const { return deficit; }
void PacketQueue::set_deficit(ms deficit) { this->deficit = deficit; }

Packet &PacketQueue::front() { return this->packet_queue.front(); }
int PacketQueue::size() const { return this->packet_queue.size(); }
void PacketQueue::pop() { this->packet_queue.pop(); }