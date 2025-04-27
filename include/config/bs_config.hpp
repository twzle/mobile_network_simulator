#pragma once

// Конфигурация базовой станции
class BSConfig
{
public:
    BSConfig();
    BSConfig(double x, double y, double z,
             double carrier_frequency, double bandwidth, int transmission_power);
    BSConfig(const BSConfig &bs_config);
    BSConfig &operator=(const BSConfig &bs_config);

    double get_x();
    double get_y();
    double get_z();
    double get_carrier_frequency();
    double get_bandwidth();
    int get_transmission_power();

private:
    double x;                 // Начальная координата x (метры)
    double y;                 // Начальная координата y (метры)
    double z;                 // Начальная координата z (метры)
    double carrier_frequency; // Несущая частота (МГц)
    double bandwidth;         // Ширина полосы пропускания (МГц)
    int transmission_power;   // Мощность передачи
};