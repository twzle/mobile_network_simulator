#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

class Mobility
{
public:
    Mobility();
    Mobility(double speed, std::string direction);
    Mobility(const Mobility &mobility);
    Mobility &operator=(const Mobility &mobility);

    double get_speed();
    std::string get_direction();

private:
    double speed; // Скорость (км/ч)
    std::string direction; // Начальная координата y (метры)
};