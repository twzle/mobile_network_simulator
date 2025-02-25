#include "core/position.hpp"

Position::Position() : x(0), y(0), z(0) {}

Position::Position(
    double x, double y, double z) : x(x), y(y), z(z)
{
}

Position::Position(const Position &position)
    : x(position.x), y(position.y), z(position.z) {}

Position & Position::operator=(const Position &position)
{
    if (this != &position)
    {
        x = position.x;
        y = position.y;
        z = position.z;
    }
    return *this;
}

double Position::get_x()
{
    return this->x;
}

double Position::get_y()
{
    return this->y;
}

double Position::get_z()
{
    return this->z;
}
