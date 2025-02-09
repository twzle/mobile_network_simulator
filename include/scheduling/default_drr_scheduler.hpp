#pragma once

#include "base_drr_scheduler.hpp"

class DefaultDRRScheduler : public BaseDRRScheduler
{
public:
    explicit DefaultDRRScheduler(
        std::string standard_name, double tti,
        double channel_sync_interval,
        std::string base_modulation_scheme);
    void run() override;

    int get_next_initial_queue();
    void set_last_starving_queue(int last_starving_queue);

private:
    int last_starving_queue = 0;
};
