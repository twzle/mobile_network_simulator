#include "core/user.hpp"

int User::last_id;

User::User() : id(++last_id) {}

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
