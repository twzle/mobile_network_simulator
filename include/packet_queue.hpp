#include "packet.hpp"

class PacketQueue {
    public:
        PacketQueue(int quant);

        void add_packet(const Packet& packet);

        ms get_quant() const;
        ms get_deficit() const;
        void set_deficit(ms deficit);

        Packet& front();
        int size() const;
        void pop();

    private:
        ms quant;
        ms deficit;
        std::queue<Packet> packet_queue;
};