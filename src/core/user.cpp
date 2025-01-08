#include "core/user.hpp"

int User::last_id;

User::User() : id(++last_id) {}

void User::initialize()
{
    last_id = 0;
}

int User::get_id() const
{
    return id;
}
