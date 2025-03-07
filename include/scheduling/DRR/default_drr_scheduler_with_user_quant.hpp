#pragma once

#include "scheduling/RR/base_rr_scheduler.hpp"

class DefaultDRRSchedulerWithUserQuant : public BaseRRScheduler
{
public:
    explicit DefaultDRRSchedulerWithUserQuant();
    void run() override;

    int get_next_initial_queue();
    void set_last_starving_queue(int last_starving_queue);

    void update_users_deficit();

private:
    int last_starving_queue = 0;
};
