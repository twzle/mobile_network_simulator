#include "user_config.hpp"

UserConfig::UserConfig(
    double x, double y, double z,
    double speed, std::string direction) : 
    x(x), y(y), z(z), speed(speed), direction(direction)
{
}

double UserConfig::get_x()
{
    return this->x;
}

double UserConfig::get_y()
{
    return this->y;
}

double UserConfig::get_z()
{
    return this->z;
}

double UserConfig::get_speed()
{
    return this->speed;
}

std::string UserConfig::get_direction()
{
    return this->direction;
}
