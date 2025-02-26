#include "core/position.hpp"

Position::Position() : x(0), y(0), z(0) {}

Position::Position(
    double x, double y, double z) : x(x), y(y), z(z)
{
}

Position::Position(const Position &position)
    : x(position.x), y(position.y), z(position.z) {}

Position &Position::operator=(const Position &position)
{
    if (this != &position)
    {
        x = position.x;
        y = position.y;
        z = position.z;
    }
    return *this;
}

double Position::get_x() const
{
    return this->x;
}

double Position::get_y() const
{
    return this->y;
}

double Position::get_z() const
{
    return this->z;
}

void Position::set_x(double x)
{
    this->x = x;
}

void Position::set_y(double y)
{
    this->y = y;
}

void Position::set_z(double z)
{
    this->z = z;
}

double Position::get_distance_2d(const Position &position)
{
    double delta_x = x - position.x;
    double delta_y = y - position.y;

    return std::sqrt(delta_x * delta_x + delta_y * delta_y);
}

double Position::get_distance_3d(const Position &position)
{
    double delta_z = z - position.z;
    double distance_2d = get_distance_2d(position);

    return std::sqrt(distance_2d * distance_2d + delta_z * delta_z);
}