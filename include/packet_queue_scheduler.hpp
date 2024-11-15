#pragma once

#include <map>
#include "packet_queue.hpp"
#include "execution_stats.hpp"

class PacketQueueScheduler {
    public:
        PacketQueueScheduler() = default;

        void run();
        void schedule(PacketQueue&& packet_queue);
        ExecutionStats& get_stats();

    private:
        int total_packets = 0; // Общее число пакетов для обслуживания во всех очередях
        std::vector<PacketQueue> scheduled_queues; // Обслуживаемые очереди
        ExecutionStats stats; // Статистика производительности планировщика
};