#pragma once
#include "base_drr_scheduler.hpp"

class FixedDRRScheduler : public BaseDRRScheduler
{
public:
    explicit FixedDRRScheduler(double tti);
    void run() override;

    int get_next_initial_queue();
};
