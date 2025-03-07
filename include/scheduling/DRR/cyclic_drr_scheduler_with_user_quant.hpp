#pragma once

#include "scheduling/RR/base_rr_scheduler.hpp"

class CyclicDRRSchedulerWithUserQuant : public BaseRRScheduler
{
public:
    explicit CyclicDRRSchedulerWithUserQuant();
    void run() override;

    int get_next_initial_queue();

    void update_users_deficit();
};
