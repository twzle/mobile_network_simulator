#include "packet_queue.hpp"

class Stats {
    public:
        Stats();
        
        void summarize();

        ms total_time; // Общее время работы планировщика в мс
        ms total_skip_time; // Общее время простоя планировщика в мс
        ms total_processing_time; // Общее время обслуживания пакетов в мс
        int packet_count; // Общее число обслуженных пакетов
        int retried_packet_count; // Число обслуженных пакетов не с первого раза
        int lost_packet_count; // Число потерянных пакетов
};

class PacketQueueScheduler {
    public:
        PacketQueueScheduler(PacketQueue& packet_queue);

        void run();
        void schedule(const PacketQueue& packet_queue);

    private:
        int total_packets = 0; // Общее число пакетов для обслуживания во всех очередях
        std::vector<PacketQueue> scheduled_queues; // Обслуживаемые очереди
        Stats stats; // Статистика производительности планировщика
};