#pragma once

#include "base_rr_scheduler.hpp"

class CyclicDRRScheduler : public BaseRRScheduler
{
public:
    explicit CyclicDRRScheduler(
        std::string standard_name, double tti,
        double channel_sync_interval,
        std::string base_modulation_scheme);
    void run() override;

    int get_next_initial_queue();
};
