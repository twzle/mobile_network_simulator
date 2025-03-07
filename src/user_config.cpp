#include "user_config.hpp"

UserConfig::UserConfig(
    double x, double y, double z,
    double speed, std::string direction,
    double quant) : 
    x(x), y(y), z(z), 
    speed(speed), direction(direction),
    quant(quant)
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

double UserConfig::get_quant()
{
    return this->quant;
}
