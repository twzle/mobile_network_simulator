#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

class Position
{
public:
    Position();
    Position(double x, double y, double z);
    Position(const Position &position);
    Position &operator=(const Position &position);

    double get_x();
    double get_y();
    double get_z();

    void set_x(double x);
    void set_y(double y);
    void set_z(double z);

private:
    double x; // Начальная координата x (метры)
    double y; // Начальная координата y (метры)
    double z; // Начальная координата z (метры)
};