#include <map>
#include "packet_queue.hpp"
#include "stats.hpp"

class PacketQueueScheduler {
    public:
        PacketQueueScheduler() = default;
        PacketQueueScheduler(PacketQueue& packet_queue);

        void run();
        void schedule(const PacketQueue& packet_queue);

    private:
        int total_packets = 0; // Общее число пакетов для обслуживания во всех очередях
        std::vector<PacketQueue> scheduled_queues; // Обслуживаемые очереди
        Stats stats; // Статистика производительности планировщика
};