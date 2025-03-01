#pragma once

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstdlib>
#include <algorithm>
#include "core/position.hpp"
#include "core/mobility.hpp"
#include "const.hpp"

class User
{
public:
    // Конструктор, который автоматически присваивает уникальный id
    User(uint8_t cqi, Position position, Mobility mobility);

    static void initialize();
    static void reset_last_id();

    int get_id() const;
    uint8_t get_cqi() const;
    void set_cqi(uint8_t cqi);

    Position get_position() const;
    void set_position(const Position &position);

    Mobility get_mobility() const;
    void set_mobility(const Mobility &mobility);

    void move(double time_in_seconds);

    double get_time_from_last_channel_sync() const;
    void set_time_from_last_channel_sync(double time_from_last_channel_sync);

    std::string get_random_move_direction();

private:
    int id;                             // Уникальный идентификатор
    uint8_t cqi;                        // CQI (индекс качества канала 0-15)
    double time_from_last_channel_sync; // Времени без синхронизации
    Position position;                  // Позиция пользователя в пространстве
    Mobility mobility;                  // Пользовательская мобильность

    static int last_id; // Статическая переменная для отслеживания последнего id
};