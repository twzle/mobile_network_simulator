#pragma once

#include <string>
#include <stdint.h>

// Конфигурация пользовательского устройства
class QueueConfig
{
public:
    QueueConfig(int packet_count, double quant);

    int get_packet_count();
    double get_quant();

private:
    int packet_count; // Число пакетов
    double quant;     // Квант времени (байт/мс)
};