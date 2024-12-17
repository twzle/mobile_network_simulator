#pragma once
#include "base_drr_scheduler.hpp"

class DefaultDRRScheduler : public BaseDRRScheduler
{
public:
    void run() override;
};
