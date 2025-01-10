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
    int size;            // Размер пакета (время на обслуживание) в ресурсных блоках (RB)
    double scheduled_at; // Время в которое для пакета было запланировано обслуживание
    int queue;           // Очередь в которую поставлен пакет
    User *user;          // Пользователь которому принадлежит пакет
};

struct PacketGreater
{
    bool operator()(const Packet &lhs, const Packet &rhs) const
    {
        return lhs.get_scheduled_at() > rhs.get_scheduled_at();
    }
};