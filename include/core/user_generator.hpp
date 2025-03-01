#pragma once

#include <random>

class UserGenerator {
    public:
        static void initialize();
        
        static int generate_user_id(); // Генерация id пользователя
        static std::uniform_int_distribution<> get_user_id_distribution(); // Получение текущего распределения
        static void set_user_id_distribution(std::uniform_int_distribution<> new_distrib); // Изменение текущего распределения
        
        static int generate_user_move_direction(); // Генерация направления движения пользователя       
        static std::uniform_int_distribution<> get_user_move_direction_distribution(); // Получение текущего распределения
        static void set_user_move_direction_distribution(std::uniform_int_distribution<> new_distrib); // Изменение текущего распределения

    private:
        static std::mt19937 user_id_gen; // Генератор
        static std::mt19937 user_move_direction_gen; // Генератор
        static std::uniform_int_distribution<> user_id_distrib; // Целочисленное равномерное распределение для нахождения id подключенного пользователя
        static std::uniform_int_distribution<> user_move_direction_distrib; // Целочисленное равномерное распределение для выбора направления движения пользователя
};