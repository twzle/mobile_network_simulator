#include "core/mobility.hpp"

Mobility::Mobility() : speed(0) {}

Mobility::Mobility(
    double speed, std::string direction) : speed(speed), direction(direction)
{
}

Mobility::Mobility(const Mobility &mobility)
    : speed(mobility.speed), direction(mobility.direction) {}

Mobility & Mobility::operator=(const Mobility &mobility)
{
    if (this != &mobility)
    {
        speed = mobility.speed;
        direction = mobility.direction;
    }
    return *this;
}

double Mobility::get_speed()
{
    return this->speed;
}

std::string Mobility::get_direction()
{
    return this->direction;
}
