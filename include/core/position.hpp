#pragma once

#include <iostream>
#include <string>
#include <stdint.h>
#include <cmath>

class Position
{
public:
    Position();
    Position(double x, double y, double z);
    Position(const Position &position);
    Position &operator=(const Position &position);

    double get_x() const;
    double get_y() const;
    double get_z() const;

    void set_x(double x);
    void set_y(double y);
    void set_z(double z);

    double get_distance_2d(const Position& position);
    double get_distance_3d(const Position& position);

    friend std::ostream &operator<<(std::ostream &os, const Position &position);

private:
    double x; // Начальная координата x (метры)
    double y; // Начальная координата y (метры)
    double z; // Начальная координата z (метры)
};