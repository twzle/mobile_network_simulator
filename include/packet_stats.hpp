#pragma once

#include "types.hpp"

class PacketStats {
    public:
        PacketStats(double scheduled_at, double processing_delay);

        double scheduled_at; // Время постановки в очередь
        double processing_delay; // Задержка обработки пакета
};
