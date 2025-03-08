#pragma once

#include <set>

#include "scheduling/base_scheduler.hpp"

class BaseRRScheduler : public BaseScheduler
{
public:
    explicit BaseRRScheduler();
    virtual ~BaseRRScheduler() = default;

    // Основной метод запуска планировщика
    virtual void run() = 0;
    // Метод для получения относительного индекса очереди (может быть переопределен)
    virtual size_t get_relative_queue_id(size_t current_absolute_queue_id);

    // Методы управления начальной очередью
    size_t get_initial_queue();
    void set_initial_queue(size_t new_initial_queue_id);

    void check_queue_remaining_scheduled_packets(
        PacketQueue &queue,
        double current_time,
        TTIStats &tti_stats);

    void check_queue_remaining_scheduled_packets_with_queue_quant(
        PacketQueue &queue,
        double current_time,
        TTIStats &tti_stats);

    void check_queue_remaining_scheduled_packets_with_user_quant(
        PacketQueue &queue,
        double current_time,
        TTIStats &tti_stats);

    void schedule(PacketQueue &&packet_queue) override;
    void sync_user_channels() override;
    void evaluate_stats() override;

protected:
    size_t current_initial_absolute_queue_id = 0; // Абсолютный ID начальной очереди
    std::vector<PacketQueue> scheduled_queues;    // Очереди для обслуживания

    std::set<User *> users_served_in_tti; // Пользователи обслуженные за TTI
};