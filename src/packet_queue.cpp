#include "packet_queue.hpp"
#include "time_generator.hpp"

PacketQueue::PacketQueue(int quant, size_t limit = 1) 
    : quant(quant), deficit(0), limit(limit) {};

void PacketQueue::add_packet(Packet &packet)
{
    if (packet_queue.size() < limit)
    {
        ms time = TimeGenerator::generate_time();
        packet.set_scheduled_at(time_point(time));
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
Packet PacketQueue::front() const { return this->packet_queue.top(); }
int PacketQueue::size() const { return this->packet_queue.size(); }
void PacketQueue::pop() { this->packet_queue.pop(); }
void PacketQueue::push(const Packet& packet) { return this->packet_queue.push(packet); }

void PacketQueue::print(){
    while (!this->packet_queue.empty()) {
        Packet packet = this->front();
        ms initial_time = 
            std::chrono::duration_cast<ms>(packet.get_scheduled_at().time_since_epoch());
        std::cout << initial_time.count() << "\n";
        this->packet_queue.pop();  // Удаляем элемент после его обработки
    }
}