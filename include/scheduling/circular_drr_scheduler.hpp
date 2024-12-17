#pragma once
#include "base_drr_scheduler.hpp"

class CircularDRRScheduler : public BaseDRRScheduler
{
public:
    explicit CircularDRRScheduler(double tti);
    void run() override;
};
