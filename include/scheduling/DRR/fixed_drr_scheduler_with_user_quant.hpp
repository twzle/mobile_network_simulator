#pragma once

#include "scheduling/RR/base_rr_scheduler.hpp"

class FixedDRRSchedulerWithUserQuant : public BaseRRScheduler
{
public:
    explicit FixedDRRSchedulerWithUserQuant();
    void run() override;

    int get_next_initial_queue();

    void update_users_deficit();
};
