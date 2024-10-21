#include "packet_stats.hpp"

PacketStats::PacketStats(ms scheduled_at, ms processing_delay)
    : scheduled_at(scheduled_at), processing_delay(processing_delay) {}