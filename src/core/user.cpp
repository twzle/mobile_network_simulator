#include "core/user.hpp"

int User::last_id;

User::User(std::string modulation_scheme)
    : id(++last_id), modulation_scheme(modulation_scheme),
      out_of_channel_sync_for(0) {}

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

std::string User::get_modulation_scheme() const
{
    return modulation_scheme;
}

void User::set_modulation_scheme(std::string modulation_scheme)
{
    this->modulation_scheme = modulation_scheme;
}

double User::get_out_of_channel_sync_for() const
{
    return out_of_channel_sync_for;
}

void User::set_out_of_channel_sync_for(double out_of_channel_sync_for)
{
    this->out_of_channel_sync_for = out_of_channel_sync_for;
}