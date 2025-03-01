#include "core/channel.hpp"

Channel::Channel()
    : carrier_frequency(0), constant_offset(0), power_bs_transmitted(0)
{
}

Channel::Channel(double carrier_frequency, int power_bs_transmitted, std::string area_type)
    : carrier_frequency(carrier_frequency),
      power_bs_transmitted(power_bs_transmitted)
{
    if (area_type == "Urban"){
        constant_offset = 3;
    } else if (area_type == "Suburban"){
        constant_offset = 0;
    } else if (area_type == "Rural"){
        constant_offset = 0;
    }
}

/*
PathLoss по модели COST231
*/
double Channel::get_path_loss(
    double user_to_bs_distance, double bs_height, double user_height)
{
    double log_freq = std::log10(carrier_frequency);
    double log_height_bs = std::log10(bs_height);
    double log_distance = std::log10(user_to_bs_distance);

    double a_height_user = (1.1 * log_freq - 0.7) * user_height - (1.56 * log_freq - 0.8);

    double path_loss =
        46.3 + 33.9 * log_freq - 13.82 * log_height_bs -
        a_height_user +
        (44.9 - 6.55 * log_height_bs) * log_distance +
        constant_offset;

    return path_loss;
}

double Channel::get_sinr(
    double user_received_signal_power,
    double noise_power, double interference_power)
{
    double sinr = user_received_signal_power - noise_power - interference_power;

    return sinr;
}

double Channel::get_recieved_signal_power(double path_loss)
{
    double power_user_received = power_bs_transmitted - path_loss;

    return power_user_received;
}

double Channel::get_noise_power()
{
    double k_Boltzmann = 1.380649e-23; // Дж/К
    double T = 290.0;                  // Температура в Кельвинах
    double B = 20e6;                   // Полоса пропускания 20 МГц

    double power_noise = 10 * log10(k_Boltzmann * T * B); // dBm

    return power_noise;
}

double Channel::get_interference_power(double user_received_signal_power)
{
    double bs_to_ue_interference_offset = -30;

    double interference_power = user_received_signal_power + bs_to_ue_interference_offset;

    return interference_power;
}