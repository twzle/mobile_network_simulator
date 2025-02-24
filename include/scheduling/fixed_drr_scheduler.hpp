#pragma once

#include "base_rr_scheduler.hpp"

class FixedDRRScheduler : public BaseRRScheduler
{
public:
    explicit FixedDRRScheduler(
        std::string standard_name, double tti,
        double channel_sync_interval,
        uint8_t base_cqi);
    void run() override;

    int get_next_initial_queue();
};
