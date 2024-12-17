#pragma once

#include <map>
#include <string>
#include <vector>

struct StandardInfo
{
    std::string name; // Название стандарта
    std::string description; // Краткое описание
    std::map<std::string, double> ttis;  // TTI для данного стандарта
    std::vector<double> bandwidths; // Допустимые полосы пропускания (в МГц)
    std::vector<std::string> schedulers; // Доступные планировщики
};

class StandardManager
{
public:
    // Статическая функция для получения информации о стандарте
    static StandardInfo get_standard_info(const std::string& standard_name);

    // Статическая функция для получения TTI по строковому ключу
    static double get_tti(const std::string& tti_key);

    // Статическая функция для инициализации данных
    static void initialize();

private:
    // Информации о стандартах
    static std::map<std::string, StandardInfo> standard_info_map;
    // Величины TTI по строковому ключу
    static std::map<std::string, double> tti_map;
};
