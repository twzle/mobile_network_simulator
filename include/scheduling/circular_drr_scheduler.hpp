#pragma once

#include "base_drr_scheduler.hpp"

class CircularDRRScheduler : public BaseDRRScheduler
{
public:
    explicit CircularDRRScheduler(
        std::string standard_name, double tti,
        double channel_sync_interval,
        std::string base_modulation_scheme);
    void run() override;

    int get_next_initial_queue();
};
