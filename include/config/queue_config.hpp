#pragma once

#include <string>
#include <stdint.h>

// Конфигурация пользовательского устройства
class QueueConfig
{
public:
    QueueConfig(int packet_count, double quant, double time_lambda);

    int get_packet_count();
    double get_quant();
    double get_time_lambda();

private:
    int packet_count; // Число пакетов
    double quant;     // Квант времени (байт/мс)
    double time_lambda; // Интенсивность прихода пакетов (пакет/с)
};