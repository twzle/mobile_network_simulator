#pragma once

#include "base_rr_scheduler.hpp"

class FixedDRRScheduler : public BaseRRScheduler
{
public:
    explicit FixedDRRScheduler(
        double tti,
        double channel_sync_interval,
        uint8_t base_cqi,
        Channel channel);
    void run() override;

    int get_next_initial_queue();
};
