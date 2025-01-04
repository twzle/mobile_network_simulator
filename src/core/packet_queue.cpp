#include "core/packet_queue.hpp"

PacketQueue::PacketQueue(int quant, size_t limit = 1)
    : quant(quant), deficit(0), limit(limit) {};

void PacketQueue::add_packet(Packet &packet)
{
    if (packet_queue.size() < limit)
    {
        double time = TimeGenerator::generate_time();
        packet.set_scheduled_at(time);
        packet_queue.push(packet);
    }
}

int PacketQueue::get_quant() const { return quant; }
int PacketQueue::get_deficit() const { return deficit; }
void PacketQueue::set_deficit(int deficit) { this->deficit = deficit; }

// Обертка над std::queue
Packet PacketQueue::front() const { return this->packet_queue.top(); }
int PacketQueue::size() const { return this->packet_queue.size(); }
void PacketQueue::pop() { this->packet_queue.pop(); }
void PacketQueue::push(const Packet &packet) { return this->packet_queue.push(packet); }

void PacketQueue::print()
{
    while (!this->packet_queue.empty())
    {
        Packet packet = this->front();
        double schedule_time = packet.get_scheduled_at();
        std::cout << schedule_time << "\n";
        this->packet_queue.pop(); // Удаляем элемент после его обработки
    }
}