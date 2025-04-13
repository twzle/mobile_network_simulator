#pragma once

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include "core/position.hpp"
#include "core/mobility.hpp"
#include "const.hpp"

class User
{
public:
    // Конструктор, который автоматически присваивает уникальный id
    User(
        uint8_t cqi,
        Position position, Mobility mobility,
        int throughput_history_size, double quant);

    static void initialize();
    static void reset_last_id();

    int get_id() const;
    uint8_t get_cqi() const;
    void set_cqi(uint8_t cqi);

    Position get_position() const;
    void set_position(const Position &position);

    Mobility get_mobility() const;
    void set_mobility(const Mobility &mobility);

    double get_priority() const;
    void set_priority(double priority);

    double get_current_throughput() const;
    double get_average_throughput() const;
    void set_current_throughput(double throughput);
    void increment_current_throughput(int rb_count);

    void initialize_throughput_history(int throughput_history_size);
    void update_throughput_history();

    bool is_resource_candidate();
    void set_resource_candidate(bool resource_candidate);

    void move(double time_in_seconds);

    double get_time_from_last_channel_sync() const;
    void set_time_from_last_channel_sync(double time_from_last_channel_sync);

    std::string get_random_move_direction();
    double get_quant() const;

    double get_deficit() const;
    void set_deficit(double deficit);

private:
    int id;                               // Уникальный идентификатор
    uint8_t cqi;                          // CQI (индекс качества канала 0-15)
    double time_from_last_channel_sync;   // Времени без синхронизации
    Position position;                    // Позиция пользователя в пространстве
    Mobility mobility;                    // Пользовательская мобильность
    double priority;                      // Приоритет (PF-метрика)
    double current_throughput;            // Текущая пропускная способность
    double average_historical_throughput; // Историческое значение пропускной способности (R)
    int throughput_history_size;          // Размер истории
    bool resource_candidate;              // Претендовал ли пользователь на ресурсы в TTI
    double quant;                         // Квант времени (RB/мс)
    double deficit;                       // Дефицит ресурсов (RB)

    static int last_id; // Статическая переменная для отслеживания последнего id
};

struct UserPFComparator
{
    bool operator()(const User *lhs, const User *rhs) const
    {
        if (lhs->get_priority() != rhs->get_priority())
        {
            return lhs->get_priority() > rhs->get_priority(); // Больше приоритет — выше
        }
        return lhs->get_average_throughput() < rhs->get_average_throughput(); // Меньший throughput — выше
    }
};