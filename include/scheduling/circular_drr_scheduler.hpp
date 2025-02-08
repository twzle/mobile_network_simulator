#pragma once

#include "base_drr_scheduler.hpp"

class CircularDRRScheduler : public BaseDRRScheduler
{
public:
    explicit CircularDRRScheduler(double tti, int rb_effective_data_size);
    void run() override;

    int get_next_initial_queue();
};
