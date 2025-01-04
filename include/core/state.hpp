#pragma once

enum class PacketQueueState
{
    UNDEFINED = 0,
    WAIT = 1,
    IDLE = 2,
    PROCESSING = 3,
};

PacketQueueState set_wait(PacketQueueState previous_state);
PacketQueueState set_idle(PacketQueueState previous_state);
PacketQueueState set_processing(PacketQueueState previous_state);

enum class SchedulerState
{
    UNDEFINED = 0,
    WAIT = 1,
    IDLE = 2,
    PROCESSING = 3,
};

SchedulerState set_wait(SchedulerState previous_state);
SchedulerState set_idle(SchedulerState previous_state);
SchedulerState set_processing(SchedulerState previous_state);