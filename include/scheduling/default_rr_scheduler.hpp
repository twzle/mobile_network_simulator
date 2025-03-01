#pragma once

#include "base_rr_scheduler.hpp"

class DefaultRRScheduler : public BaseRRScheduler
{
public:
    explicit DefaultRRScheduler(
        double tti,
        double channel_sync_interval,
        uint8_t base_cqi);
    void run() override;

    int get_next_initial_queue();
    void set_last_starving_queue(int last_starving_queue);

private:
    int last_starving_queue = 0;
};
