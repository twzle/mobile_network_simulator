#pragma once

#include <iostream>
#include <string>
#include <stdint.h>
#include "core/position.hpp"
#include "core/mobility.hpp"

class BaseStation
{
public:
    BaseStation();
    // Конструктор, который автоматически присваивает уникальный id
    BaseStation(Position position);

    Position get_position() const;
    void set_position(const Position &position);

private:
    Position position;              // Позиция пользователя в пространстве
};