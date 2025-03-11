#pragma once

#include "scheduling/base_scheduler.hpp"
#include "core/relevant_packet_queue.hpp"

class BasePFScheduler : public BaseScheduler
{
public:
    explicit BasePFScheduler();

    // Основной метод запуска планировщика
    void run();

    void flush_user_context();
    void update_user_priorities();
    void update_user_throughputs();
    void exclude_users_from_scheduling();
    void collect_relevant_packets(double current_time, TTIStats& tti_stats);
    void filter_packets_of_excluded_from_scheduling_users();

    void schedule(PacketQueue &&packet_queue) override;
    void evaluate_stats() override;
    void sync_user_channels() override;

    void reset_served_users();

protected:
    PacketQueue main_queue;              // Основная очередь для обслуживания
    RelevantPacketQueue relevant_queue;  // Вспомогательная очередь пакетов готовых к обслуживанию

    /*
    Пользователи-кандидаты на получения ресурсов 
    с сортировкой по (PF-метрике -> пропускной способности)
    */
    std::vector<User*> sorted_resource_candidates_for_tti;
};