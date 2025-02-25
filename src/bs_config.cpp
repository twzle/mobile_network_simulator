#include "bs_config.hpp"

BSConfig::BSConfig() : x(0), y(0), z(0) {}

BSConfig::BSConfig(
    double x, double y, double z) : x(x), y(y), z(z)
{
}

BSConfig::BSConfig(const BSConfig &bs_config)
    : x(bs_config.x), y(bs_config.y), z(bs_config.z) {}

BSConfig & BSConfig::operator=(const BSConfig &bs_config)
{
    if (this != &bs_config)
    {
        x = bs_config.x;
        y = bs_config.y;
        z = bs_config.z;
    }
    return *this;
}

double BSConfig::get_x()
{
    return this->x;
}

double BSConfig::get_y()
{
    return this->y;
}

double BSConfig::get_z()
{
    return this->z;
}
