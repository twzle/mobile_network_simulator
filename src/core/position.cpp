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

double Position::constrain_position(
    double pos, double move_delta, double min_limit, double max_limit, double epsilon)
{
    double new_pos = pos + move_delta;

    if (new_pos > -min_limit + epsilon && new_pos < min_limit - epsilon)
    {
        return (pos > 0) ? min_limit : -min_limit;
    }
    else if (new_pos > max_limit + epsilon)
    {
        return max_limit;
    }
    else if (new_pos < -max_limit - epsilon)
    {
        return -max_limit;
    }

    return new_pos;
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

std::ostream &operator<<(std::ostream &os, const Position &position)
{
    os << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")";
    return os;
}