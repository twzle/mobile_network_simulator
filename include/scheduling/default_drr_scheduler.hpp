#pragma once
#include "base_drr_scheduler.hpp"

class DefaultDRRScheduler : public BaseDRRScheduler
{
public:
    explicit DefaultDRRScheduler(double tti);
    void run() override;
};
