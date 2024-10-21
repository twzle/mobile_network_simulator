#pragma once

#include <map>
#include "packet_queue.hpp"
#include "execution_stats.hpp"

class PacketQueueScheduler {
    public:
        PacketQueueScheduler() = default;
        PacketQueueScheduler(PacketQueue& packet_queue);

        void run();
        void schedule(const PacketQueue& packet_queue);
        ExecutionStats get_stats() const;

    private:
        int total_packets = 0; // Общее число пакетов для обслуживания во всех очередях
        std::vector<PacketQueue> scheduled_queues; // Обслуживаемые очереди
        ExecutionStats stats; // Статистика производительности планировщика
};