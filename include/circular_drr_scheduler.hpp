#pragma once
#include "base_drr_scheduler.hpp"

class CircularDRRScheduler : public BaseDRRScheduler
{
public:
    void run() override;
};
