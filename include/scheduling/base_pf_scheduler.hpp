#pragma once

#include "scheduling/base_scheduler.hpp"
#include "core/relevant_packet_queue.hpp"

class BasePFScheduler : public BaseScheduler
{
public:
    explicit BasePFScheduler();

    // Основной метод запуска планировщика
    void run();

    void evaluate_stats() override;

protected:
    void sync_user_channels() override;

    void update_user_priorities();
    void collect_relevant_packets(double current_time, TTIStats& tti_stats);

protected:
    PacketQueue main_queue;              // Основная очередь для обслуживания
    RelevantPacketQueue relevant_queue;  // Вспомогательная очередь пакетов готовых к обслуживанию
};