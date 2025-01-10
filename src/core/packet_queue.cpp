#include "core/packet_queue.hpp"

PacketQueue::PacketQueue(double quant, size_t limit = 1)
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

double PacketQueue::get_quant() const { return quant; }
double PacketQueue::get_deficit() const { return deficit; }
void PacketQueue::set_deficit(double deficit)
{
    if (deficit <= DEFICIT_MAX)
        this->deficit = deficit;
}

// Обертка над std::queue
Packet PacketQueue::front() const { return this->packet_queue.top(); }
int PacketQueue::size() const { return this->packet_queue.size(); }
void PacketQueue::pop() { this->packet_queue.pop(); }
void PacketQueue::push(const Packet &packet) { return this->packet_queue.push(packet); }

void PacketQueue::print()
{
    int i = 0;

    std::priority_queue<Packet, std::vector<Packet>, PacketGreater> tmp;

    while (!this->packet_queue.empty())
    {   
        Packet packet = this->front();
        std::cout << "PACKET = " << i << "\n"
                  << "SCHEDULE TIME = " << packet.get_scheduled_at() << "\n"
                  << "USER = " << packet.get_user_ptr()->get_id() << "\n\n";
        this->packet_queue.pop(); // Удаляем элемент после его обработки
        tmp.push(packet);

        ++i;
    }

    while (!tmp.empty())
    {   
        Packet packet = tmp.top();
        
        tmp.pop(); // Удаляем элемент после его обработки
        this->packet_queue.push(packet);
    }
}