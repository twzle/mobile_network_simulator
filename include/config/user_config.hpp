#pragma once

#include <string>
#include <stdint.h>

// Конфигурация пользовательского устройства
class UserConfig
{
public:
    UserConfig(
        double x, double y, double z,
        double speed, std::string direction,
        double quant, int packet_size, double traffic_part);

    double get_x();
    double get_y();
    double get_z();
    double get_speed();
    std::string get_direction();
    double get_quant();
    int get_packet_size();
    double get_traffic_part();

private:
    double x;              // Начальная координата x (метры)
    double y;              // Начальная координата y (метры)
    double z;              // Начальная координата z (метры)
    double speed;          // Скорость (км/ч)
    std::string direction; // Направление движения (вперед "forward", назад "back", налево "left", направо "right")
    double quant;          // Квант времени (RB/мс)
    int packet_size;       // Размер пакета создаваемый пользователем (байт)
    double traffic_part;   // Доля в общем трафике (вероятность создания пакета пользователем) [0, 1]
};