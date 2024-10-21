#pragma once

#include <random>
#include "types.hpp"

class TimeGenerator {
    public:
        static void initialize();
        static void synchronize_time(); // Обновление относительного начального времени для повтора симуляции
        static ms generate_time(); // Генерация случайного времени
        static ms get_initial_time(); // Получение текущего относительного начального времени
        static std::uniform_int_distribution<> get_distribution(); // Получение текущего распределения

    private:
        static std::mt19937 gen; // Генератор
        static ms initial_time; // Относительное начальное время для планирования обслуживания пакетов
        static std::uniform_int_distribution<> distrib; // Равномерное распределение для прихода пакетов в очереди
};