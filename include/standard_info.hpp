#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

struct StandardInfo
{
    std::string name; // Название стандарта
    std::string description; // Краткое описание
    std::map<std::string, double> ttis;  // Доступные интервалы TTI (строковое обозначение к числу секунд)
    std::map<std::string, uint8_t> modulation_schemes;  // Схемы модуляции (строковое обозначение к числу бит полезных данных в RB)
    std::vector<double> bandwidths; // Доступные полосы пропускания (в МГц)
    std::vector<std::string> schedulers; // Доступные планировщики
    uint8_t resource_elements; // Количество доступных OFDMA-символов в одном ресурсном блоке (RB)
};

class StandardManager
{
public:
    // Статическая функция для получения информации о стандарте
    static StandardInfo get_standard_info(const std::string& standard_name);

    // Статическая функция для получения TTI по строковому ключу
    static double get_tti(
        const std::string& standard_name, 
        const std::string& tti_name);

    // Статическая функция для получения схемы модуляции по строковому ключу
    static uint8_t get_modulation_scheme(
        const std::string& standard_name, 
        const std::string& modulation_scheme_name);

    // Статическая функция для получения количества ресурсных элементов по строковому ключу
    static uint8_t get_resource_elements(
        const std::string& standard_name);

    // Статическая функция для инициализации данных
    static void initialize();

private:
    // Информации о стандартах
    static std::map<std::string, StandardInfo> standard_info_map;
};
