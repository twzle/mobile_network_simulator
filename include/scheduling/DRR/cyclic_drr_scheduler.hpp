#pragma once

#include "scheduling/RR/base_rr_scheduler.hpp"

class CyclicDRRScheduler : public BaseRRScheduler
{
public:
    explicit CyclicDRRScheduler();
    void run() override;

    int get_next_initial_queue();
};
