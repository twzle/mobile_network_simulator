#include "core/relevant_packet_queue.hpp"

RelevantPacketQueue::RelevantPacketQueue(){};


// Обертка над std::queue
Packet RelevantPacketQueue::front() const { return this->packet_queue.top(); }
int RelevantPacketQueue::size() const { return this->packet_queue.size(); }
void RelevantPacketQueue::pop() { this->packet_queue.pop(); }
void RelevantPacketQueue::push(const Packet &packet) { return this->packet_queue.push(packet); }

void RelevantPacketQueue::print()
{
    int i = 0;

    std::priority_queue<Packet, std::vector<Packet>, PacketGreaterByUserPriorityAndTime> tmp;

    while (!this->packet_queue.empty())
    {   
        Packet packet = this->front();
        std::cout << "PACKET = " << i << "\n"
                  << "SCHEDULE TIME = " << packet.get_scheduled_at() << "\n"
                  << "USER = " << packet.get_user_ptr()->get_id() << "\n"
                  << "PRIORITY = " << packet.get_user_ptr()->get_priority() << "\n"
                  << "TPUT = " << packet.get_user_ptr()->get_average_throughput() << "\n\n";
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