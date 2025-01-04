#include "core/state.hpp"

PacketQueueState set_wait(PacketQueueState previous_state)
{
    if (previous_state == PacketQueueState::UNDEFINED)
    {
        return PacketQueueState::WAIT;
    }
    return previous_state;
}

PacketQueueState set_idle(PacketQueueState previous_state)
{
    if (previous_state == PacketQueueState::UNDEFINED)
    {
        return PacketQueueState::IDLE;
    }
    return previous_state;
}

PacketQueueState set_processing(PacketQueueState previous_state)
{
    if (previous_state == PacketQueueState::UNDEFINED)
    {
        return PacketQueueState::PROCESSING;
    }
    return previous_state;
}

SchedulerState set_wait(SchedulerState previous_state)
{
    if (previous_state < SchedulerState::WAIT)
    {
        return SchedulerState::WAIT;
    }
    return previous_state;
}

SchedulerState set_idle(SchedulerState previous_state)
{
    if (previous_state < SchedulerState::IDLE)
    {
        return SchedulerState::IDLE;
    }
    return previous_state;
}

SchedulerState set_processing(SchedulerState previous_state)
{
    if (previous_state < SchedulerState::PROCESSING)
    {
        return SchedulerState::PROCESSING;
    }
    return previous_state;
}