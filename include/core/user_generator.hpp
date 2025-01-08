#pragma once

#include <random>

class UserGenerator {
    public:
        static void initialize();
        static int generate_user_id(); // Генерация id пользователя
        static std::uniform_int_distribution<> get_distribution(); // Получение текущего распределения
        static void set_distribution(std::uniform_int_distribution<> new_distrib); // Изменение текущего распределения

    private:
        static std::mt19937 gen; // Генератор
        static std::uniform_int_distribution<> distrib; // Целочисленное равномерное распределение для нахождения подключенного пользователя
};