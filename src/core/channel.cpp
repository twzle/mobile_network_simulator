#include "core/channel.hpp"

Channel::Channel()
    : carrier_frequency(0), constant_offset(0), power_bs_transmitted(0), power_interference(-120)
{
}

Channel::Channel(
    double carrier_frequency, 
    int power_bs_transmitted, 
    std::string area_type)
    : carrier_frequency(carrier_frequency),
      power_bs_transmitted(power_bs_transmitted)
{
    if (area_type == "Dense Urban")
    {
        constant_offset = 3;
        power_interference = -120;
    }
    else if (area_type == "Urban")
    {
        constant_offset = 0;
        power_interference = -120;
    }
    else if (area_type == "Suburban")
    {
        double log_frequency = log10(carrier_frequency / 28);
        constant_offset = -(2 * log_frequency * log_frequency + 5.4);
        power_interference = -120;
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

double Channel::get_snr(double user_received_signal_power, double noise_power) {
    return user_received_signal_power - noise_power; // SNR в dB
}

double Channel::get_sinr(
    double user_received_signal_power,
    double noise_power,
    double interference_power)
{
    // Переводим все величины из dBm в линейные (mW)
    double signal_mW = pow(10, user_received_signal_power / 10);
    double noise_mW = pow(10, noise_power / 10);
    double interference_mW = pow(10, interference_power / 10);

    // Вычисляем SINR в линейной форме
    double sinr_linear = signal_mW / (noise_mW + interference_mW);

    // Перевод в dB
    return 10 * log10(sinr_linear);
}

double Channel::get_received_signal_power(double path_loss)
{
    double power_user_received = power_bs_transmitted - path_loss;

    return power_user_received;
}

double Channel::get_noise_power() {
    const double k_Boltzmann = 1.380649e-23;     // Дж/К
    const double T = 290.0;                      // Температура в Кельвинах
    const double B = StandardManager::get_bandwidth() * 1e6; // Полоса в Гц

    // Правильный расчёт: 10*log10(kBTB [Вт]) + 30 -> перевод в dBm
    return 10 * log10(k_Boltzmann * T * B) + 30; // Пример: -101 dBm для 20 МГц
}

double Channel::get_interference_power() {
    return power_interference; // Уже задано для Dense Urban/Urban/Suburban
}

double Channel::get_carrier_frequency()
{
    return carrier_frequency;
}

double Channel::get_constant_offset()
{
    return constant_offset;
}

int Channel::get_power_bs_transmitted()
{
    return power_bs_transmitted;
}