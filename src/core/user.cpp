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

double User::get_out_of_channel_sync_for() const
{
    return out_of_channel_sync_for;
}

void User::set_out_of_channel_sync_for(double out_of_channel_sync_for)
{
    this->out_of_channel_sync_for = out_of_channel_sync_for;
}