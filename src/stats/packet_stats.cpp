#include "stats/packet_stats.hpp"

PacketStats::PacketStats(int queue_id, double processing_delay)
    : queue_id(queue_id), processing_delay(processing_delay) {}