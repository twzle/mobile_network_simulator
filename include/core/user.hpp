#pragma once

#include <iostream>
#include <string>

class User
{
public:
    // Конструктор, который автоматически присваивает уникальный id
    User(std::string modulation_scheme);

    static void initialize();
    static void reset_last_id();

    int get_id() const;
    std::string get_modulation_scheme() const;
    void set_modulation_scheme(std::string modulation_scheme);

    double get_out_of_channel_sync_for() const;
    void set_out_of_channel_sync_for(double out_of_channel_sync_for);

private:
    int id;                         // Уникальный идентификатор
    std::string modulation_scheme;  // Схема модуляции
    double out_of_channel_sync_for; // Времени без синхронизации

    static int last_id; // Статическая переменная для отслеживания последнего id
};