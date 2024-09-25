#include "packet_queue.hpp"

PacketQueue::PacketQueue(int quant, size_t limit = 1) 
    : quant(quant), deficit(0), limit(limit) {};

void PacketQueue::add_packet(const Packet &packet)
{
    if (packet_queue.size() < limit)
    {
        packet_queue.push(packet);
    } else {
        ++lost_packet_count;
    }
}

int PacketQueue::get_lost_packet_count() const { return lost_packet_count; }
ms PacketQueue::get_quant() const { return quant; }
ms PacketQueue::get_deficit() const { return deficit; }
void PacketQueue::set_deficit(ms deficit) { this->deficit = deficit; }

// Обертка над std::queue
Packet &PacketQueue::front() { return this->packet_queue.front(); }
int PacketQueue::size() const { return this->packet_queue.size(); }
void PacketQueue::pop() { this->packet_queue.pop(); }