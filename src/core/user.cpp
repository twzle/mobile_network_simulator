#include "core/user.hpp"
#include "core/user_generator.hpp"
#include "standard_info.hpp"
#include <algorithm>
#include <iostream>

int User::last_id;

User::User(uint8_t cqi, Position position, Mobility mobility, int throughput_history_size)
    : id(++last_id), cqi(cqi),
      time_from_last_channel_sync(0), position(position), mobility(mobility)
{
    initialize_throughput_history(throughput_history_size);
}

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

double User::get_priority() const
{
    return this->priority;
}

void User::set_priority(double priority)
{
    this->priority = priority;
}

double User::get_average_throughput() const
{
    return average_historical_throughput;
}

void User::initialize_throughput_history(int throughput_history_size)
{
    average_historical_throughput = 0.1;
    this->throughput_history_size = throughput_history_size;
}

void User::update_throughput_history(double throughput)
{
    // alpha = 2 / (N + 1)
    double alpha = 2.0 / (double)(throughput_history_size + 1);

    // R_i = (1 - alpha) * R_i + alpha * r_i
    average_historical_throughput =
        (1 - alpha) * average_historical_throughput + alpha * throughput;
}

void User::move(double time_in_seconds)
{
    if (mobility.get_speed() <= epsilon)
    {
        return;
    }

    double speed_in_m_per_ms = mobility.get_speed() / (3.6 * 1000);
    double time_in_ms = time_in_seconds * 1000;
    double move_delta = speed_in_m_per_ms * time_in_ms;

    double max_distance = BS_TO_UE_DISTANCE_MAX;
    double min_distance = BS_TO_UE_DISTANCE_MIN;

    std::string move_direction = mobility.get_direction();
    if (move_direction == "random")
    {
        move_direction = get_random_move_direction();
    }

    double old_x = position.get_x();
    double old_y = position.get_y();
    double new_x = old_x;
    double new_y = old_y;

    if (move_direction == "forward")
    {
        new_y += move_delta;

        double max_allowed_y = std::sqrt(max_distance * max_distance - old_x * old_x);
        double min_allowed_y = std::sqrt(min_distance * min_distance - old_x * old_x);

        if (new_y > max_allowed_y + epsilon)
        {
            new_y = max_allowed_y;
        }
        else if (new_y < -min_allowed_y - epsilon)
        {
            new_y = -min_allowed_y;
        }
    }
    else if (move_direction == "backward")
    {
        new_y -= move_delta;

        double max_allowed_y = std::sqrt(max_distance * max_distance - old_x * old_x);
        double min_allowed_y = std::sqrt(min_distance * min_distance - old_x * old_x);

        if (new_y > max_allowed_y + epsilon)
        {
            new_y = max_allowed_y;
        }
        else if (new_y < -min_allowed_y - epsilon)
        {
            new_y = -min_allowed_y;
        }
    }
    else if (move_direction == "right")
    {
        new_x += move_delta;

        double max_allowed_x = std::sqrt(max_distance * max_distance - old_y * old_y);
        double min_allowed_x = std::sqrt(min_distance * min_distance - old_y * old_y);

        if (new_x > max_allowed_x + epsilon)
        {
            new_x = max_allowed_x;
        }
        else if (new_x < -min_allowed_x - epsilon)
        {
            new_x = -min_allowed_x;
        }
    }
    else if (move_direction == "left")
    {
        new_x -= move_delta;

        double max_allowed_x = std::sqrt(max_distance * max_distance - old_y * old_y);
        double min_allowed_x = std::sqrt(min_distance * min_distance - old_y * old_y);

        if (new_x > max_allowed_x + epsilon)
        {
            new_x = max_allowed_x;
        }
        else if (new_x < -min_allowed_x - epsilon)
        {
            new_x = -min_allowed_x;
        }
    }

    position.set_x(new_x);
    position.set_y(new_y);
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