#include "stats/packet_stats.hpp"

PacketStats::PacketStats(int queue_id, double scheduled_at, double processing_delay)
    : queue_id(queue_id), scheduled_at(scheduled_at), processing_delay(processing_delay) {}