#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

class User
{
public:
    // Конструктор, который автоматически присваивает уникальный id
    User(uint8_t cqi);

    static void initialize();
    static void reset_last_id();

    int get_id() const;
    uint8_t get_cqi() const;
    void set_cqi(uint8_t cqi);

    double get_out_of_channel_sync_for() const;
    void set_out_of_channel_sync_for(double out_of_channel_sync_for);

private:
    int id;                         // Уникальный идентификатор
    uint8_t cqi;                    // CQI (индекс качества канала 0-15)
    double out_of_channel_sync_for; // Времени без синхронизации

    static int last_id; // Статическая переменная для отслеживания последнего id
};