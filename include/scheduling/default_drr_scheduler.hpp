#pragma once

#include "base_drr_scheduler.hpp"

class DefaultDRRScheduler : public BaseDRRScheduler
{
public:
    explicit DefaultDRRScheduler(double tti, int rb_effective_data_size);
    void run() override;

    int get_next_initial_queue();
    void set_last_starving_queue(int last_starving_queue);

private:
    int last_starving_queue = 0;
};
