#pragma once

#include <iostream>
#include <string>

class User
{
public:
    // Конструктор, который автоматически присваивает уникальный id
    User();

    static void initialize();

    int get_id() const;

private:
    int id; // Уникальный идентификатор

    static int last_id; // Статическая переменная для отслеживания последнего id
};