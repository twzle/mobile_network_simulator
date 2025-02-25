#pragma once

#include <string>
#include <stdint.h>

// Конфигурация пользовательского устройства
class UserConfig
{
public:
    UserConfig(double x, double y, double z, double speed, std::string direction);

    double get_x();
    double get_y();
    double get_z();
    double get_speed();
    std::string get_direction();

private:
    double x;              // Начальная координата x (метры)
    double y;              // Начальная координата y (метры)
    double z;              // Начальная координата z (метры)
    double speed;          // Скорость (км/ч)
    std::string direction; // Направление движения (вперед "forward", назад "back", налево "left", направо "right")
};