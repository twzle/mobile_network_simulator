#pragma once

#include "types.hpp"

class PacketStats {
    public:
        PacketStats(ms scheduled_at, ms processing_delay);

        ms scheduled_at; // Время постановки в очередь
        ms processing_delay; // Задержка обработки пакета
};
