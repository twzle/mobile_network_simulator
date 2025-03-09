#pragma once

#include "const.hpp"

class PacketStats {
    public:
        PacketStats(int queue_id, int user_id, double processing_delay);
        PacketStats(const PacketStats&) = delete;
        PacketStats(PacketStats&&) = default;

        int queue_id; // Очередь в которой находился пакет
        int user_id; // Пользователь которому принадлежал пакет
        double processing_delay; // Задержка обработки пакета (секунды)
};
