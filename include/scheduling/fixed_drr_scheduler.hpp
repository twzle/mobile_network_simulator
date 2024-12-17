#pragma once
#include "base_drr_scheduler.hpp"

class FixedDRRScheduler : public BaseDRRScheduler
{
public:
    void run() override;
};
