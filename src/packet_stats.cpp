#include "packet_stats.hpp"

PacketStats::PacketStats(double scheduled_at, double processing_delay)
    : scheduled_at(scheduled_at), processing_delay(processing_delay) {}