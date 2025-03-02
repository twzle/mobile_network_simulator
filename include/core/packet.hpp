#pragma once

#include <iostream>
#include <queue>
#include <thread>

#include "const.hpp"
#include "core/user.hpp"

class Packet
{
public:
    Packet(int size, int queue, User *user);

    int get_size() const;
    void set_size(int size);

    int get_queue() const;
    void set_queue(int queue);

    User* get_user_ptr() const;
    void set_user_ptr(User* user);

    double get_scheduled_at() const;
    void set_scheduled_at(double scheduled_at);

private:
    int size;            // Размер пакета (байты)
    double scheduled_at; // Время прихода пакета в систему (секунды)
    int queue;           // Очередь в которую пришел пакет
    User *user;          // Пользователь создавший пакет
};

struct PacketGreaterByTime
{
    bool operator()(const Packet &lhs, const Packet &rhs) const
    {
        return lhs.get_scheduled_at() > rhs.get_scheduled_at();
    }
};

struct PacketGreaterByUserPriorityAndTime
{
    bool operator()(const Packet &lhs, const Packet &rhs) const
    {
        double lhs_priority = lhs.get_user_ptr()->get_priority();
        double rhs_priority = rhs.get_user_ptr()->get_priority();

        if (std::fabs(lhs_priority - rhs_priority) < epsilon) {
            // Приоритеты практически равны → сортируем по времени (по возрастанию)
            return lhs.get_scheduled_at() > rhs.get_scheduled_at();
        } else {
            // Приоритеты разные → сортируем по убыванию приоритета
            return lhs_priority > rhs_priority;
        }
    }
};