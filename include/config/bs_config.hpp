#pragma once

// Конфигурация базовой станции
class BSConfig
{
public:
    BSConfig();
    BSConfig(double x, double y, double z);
    BSConfig(const BSConfig &bs_config);
    BSConfig &operator=(const BSConfig &bs_config);

    double get_x();
    double get_y();
    double get_z();

private:
    double x; // Начальная координата x (метры)
    double y; // Начальная координата y (метры)
    double z; // Начальная координата z (метры)
};