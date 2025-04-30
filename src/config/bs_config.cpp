#include "config/bs_config.hpp"

BSConfig::BSConfig() : 
    x(0), y(0), z(0), 
    carrier_frequency(0), bandwidth(0), transmission_power(0) 
{}

BSConfig::BSConfig(
    double x, double y, double z, 
    double carrier_frequency, double bandwidth, int transmission_power) : 
    x(x), y(y), z(z),
    carrier_frequency(carrier_frequency), bandwidth(bandwidth), 
    transmission_power(transmission_power)
{
}

BSConfig::BSConfig(const BSConfig &bs_config)
    : x(bs_config.x), y(bs_config.y), z(bs_config.z),
    carrier_frequency(bs_config.carrier_frequency), bandwidth(bs_config.bandwidth), 
    transmission_power(bs_config.transmission_power)
{}

BSConfig & BSConfig::operator=(const BSConfig &bs_config)
{
    if (this != &bs_config)
    {
        x = bs_config.x;
        y = bs_config.y;
        z = bs_config.z;
        carrier_frequency = bs_config.carrier_frequency;
        bandwidth = bs_config.bandwidth;
        transmission_power = bs_config.transmission_power;
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

double BSConfig::get_carrier_frequency()
{
    return carrier_frequency;
}

double BSConfig::get_bandwidth()
{
    return bandwidth;
}

int BSConfig::get_transmission_power()
{
    return transmission_power;
}
