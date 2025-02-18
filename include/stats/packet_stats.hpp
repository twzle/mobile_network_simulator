#pragma once

#include "const.hpp"

class PacketStats {
    public:
        PacketStats(int queue_id, double processing_delay);
        PacketStats(const PacketStats&) = delete;
        PacketStats(PacketStats&&) = default;

        int queue_id; // Очередь в которой находился пакет
        double processing_delay; // Задержка обработки пакета (секунды)
};
