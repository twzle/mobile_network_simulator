#pragma once

#include "base_rr_scheduler.hpp"

class FixedDRRScheduler : public BaseRRScheduler
{
public:
    explicit FixedDRRScheduler();
    void run() override;

    int get_next_initial_queue();
};
