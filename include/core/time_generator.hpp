#pragma once

#include <random>

#include "const.hpp"

class TimeGenerator {
    public:
        static void initialize();
        static void reset_time(); // Обновление относительного начального времени для повтора симуляции
        static double generate_time(); // Генерация случайного времени
        static double get_initial_time(); // Получение текущего относительного начального времени
        static std::exponential_distribution<> get_distribution(); // Получение текущего распределения
        static void set_distribution(std::exponential_distribution<> new_distrib); // Получение текущего распределения

    private:
        static std::mt19937 gen; // Генератор
        static double initial_time; // Относительное начальное время для планирования обслуживания пакетов
        static double last_scheduling_time; // Относительное время прихода последнего пакета 
        static std::exponential_distribution<> distrib; // Равномерное распределение для прихода пакетов в очереди
};