#include "core/user.hpp"
#include "core/user_generator.hpp"
#include "standard_info.hpp"
#include <algorithm>
#include <iostream>

int User::last_id;

User::User(uint8_t cqi, Position position, Mobility mobility)
    : id(++last_id), cqi(cqi),
      time_from_last_channel_sync(0), position(position), mobility(mobility) {}

void User::initialize()
{
    reset_last_id();
}

void User::reset_last_id()
{
    last_id = 0;
}

int User::get_id() const
{
    return id;
}

uint8_t User::get_cqi() const
{
    return cqi;
}

void User::set_cqi(uint8_t cqi)
{
    this->cqi = cqi;
}

Position User::get_position() const
{
    return position;
}

void User::set_position(const Position &position)
{
    this->position = position;
}

Mobility User::get_mobility() const
{
    return mobility;
}

void User::set_mobility(const Mobility &mobility)
{
    this->mobility = mobility;
}

void User::move(double time_in_seconds)
{
    // Если скорость 0 км/ч или меньше epsilon, пользователь не двигается
    if (mobility.get_speed() <= epsilon)
    {
        return;
    }

    // Конвертация скорости из км/ч в м/мс
    double speed_in_m_per_ms = mobility.get_speed() / (3.6 * 1000); // (км/ч -> м/мс)
    double time_in_ms = time_in_seconds * 1000;
    double move_delta = speed_in_m_per_ms * time_in_ms;

    double x_bound = CELL_COVERAGE;
    double y_bound = CELL_COVERAGE;

    std::string move_direction = mobility.get_direction();

    if (move_direction == "random")
    {
        move_direction = get_random_move_direction();
    }

    if (move_direction == "forward")
    {
        double new_y = position.get_y() + move_delta;
        position.set_y(std::min(new_y, y_bound));
    }
    else if (move_direction == "backward")
    {
        double new_y = position.get_y() - move_delta;
        position.set_y(std::max(new_y, -y_bound));
    }
    else if (move_direction == "right")
    {
        double new_x = position.get_x() + move_delta;
        position.set_x(std::min(new_x, x_bound));
    }
    else if (move_direction == "left")
    {
        double new_x = position.get_x() - move_delta;
        position.set_x(std::max(new_x, -y_bound));
    }
}

std::string User::get_random_move_direction()
{
    // Random mobility id: 1 - forward, 2 - backward, 3 - left, 4 - right
    int move_direction_id = UserGenerator::generate_user_move_direction();
    return StandardManager::get_mobility_direction(move_direction_id);
}

double User::get_time_from_last_channel_sync() const
{
    return time_from_last_channel_sync;
}

void User::set_time_from_last_channel_sync(double time_from_last_channel_sync)
{
    this->time_from_last_channel_sync = time_from_last_channel_sync;
}