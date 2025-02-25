#include "core/user.hpp"

int User::last_id;

User::User(uint8_t cqi, Position position, Mobility mobility)
    : id(++last_id), cqi(cqi),
      out_of_channel_sync_for(0), position(position), mobility(mobility) {}

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

Mobility User::get_mobility() const {
    return mobility;
}

void User::set_mobility(const Mobility &mobility){
    this->mobility = mobility;
}

void User::move(double time_in_seconds){
    // Если скорость 0 км/ч пользователь не двигается
    if (std::abs(mobility.get_speed()) < epsilon){
        return;
    }

    // Скорость (км/ч) -> (м/мс)
    double speed_in_m_for_ms = mobility.get_speed() * 1000 / (3600 * 1000);
    double time_in_ms = time_in_seconds * 1000;

    double move_delta = speed_in_m_for_ms * time_in_ms;

    if (mobility.get_direction() == "forward"){
        double new_x = position.get_x() + move_delta;

        if (new_x < CELL_COVERAGE + epsilon){
            position.set_x(new_x);    
        }
    } else if (mobility.get_direction() = "backward"){
        double new_x = position.get_x() - move_delta;

        if (new_x > -(CELL_COVERAGE + epsilon)){
            position.set_x(new_x);    
        }
    } else if (mobility.get_direction() == "right"){
        double new_y = position.get_y() + move_delta;

        if (new_y < CELL_COVERAGE + epsilon){
            position.set_y(new_y);    
        }
    } else if (mobility.get_direction() == "left"){
        double new_y = position.get_y() - move_delta;

        if (new_y > -(CELL_COVERAGE + epsilon)){
            position.set_y(new_y);    
        }
    }
}

double User::get_out_of_channel_sync_for() const
{
    return out_of_channel_sync_for;
}

void User::set_out_of_channel_sync_for(double out_of_channel_sync_for)
{
    this->out_of_channel_sync_for = out_of_channel_sync_for;
}