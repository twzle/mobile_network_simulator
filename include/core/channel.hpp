#pragma once

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstdlib>
#include "core/position.hpp"
#include "core/mobility.hpp"
#include "config/standard_info.hpp"
#include "const.hpp"

class Channel
{
public:
    Channel();
    Channel(double carrier_frequency, int power_bs_transmitted, std::string area_type);

    double get_path_loss(double user_distance, double bs_height, double user_height);
    double get_snr(double user_received_signal_power, double noise_power);
    double get_sinr(double user_received_signal_power, double noise_power, 
        double interference_power);
    double get_received_signal_power(double path_loss);
    double get_noise_power();
    double get_interference_power();

    double get_carrier_frequency();
    double get_constant_offset();
    int get_power_bs_transmitted();


private:
    double carrier_frequency; // Несущая частота в МГц
    
    /*
    Параметр Сm в дБ
    (3 дБ - плотная городская застройка, 0 дБ - городская застройка,
    -8 дБ - пригород, -15 дБ - сельская местность)
    */
    double constant_offset;

    int power_bs_transmitted; // Мощность сигнала базовой станции (46 дБ)
    double power_interference; // Мощность интерференции соседних сот (дБ)
};