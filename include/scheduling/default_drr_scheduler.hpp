#pragma once

#include "base_rr_scheduler.hpp"

class DefaultDRRScheduler : public BaseRRScheduler
{
public:
    explicit DefaultDRRScheduler();
    void run() override;

    int get_next_initial_queue();
    void set_last_starving_queue(int last_starving_queue);

private:
    int last_starving_queue = 0;
};
