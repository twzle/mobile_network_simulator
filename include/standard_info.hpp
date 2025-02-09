#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

struct StandardInfo
{
    std::string name;                                     // Название стандарта
    std::string description;                              // Краткое описание
    std::map<std::string, double> ttis;                   // Доступные интервалы TTI (строковое обозначение к числу секунд)
    std::map<std::string, double> channel_sync_intervals; // Доступные интервалы синхронизации канала (строковое обозначение к числу секунд)
    std::map<std::string, uint8_t> modulation_schemes;    // Схемы модуляции (строковое обозначение к числу бит полезных данных в OFDMA-символе (RE))
    std::vector<double> bandwidths;                       // Доступные полосы пропускания (в МГц)
    std::vector<std::string> schedulers;                  // Доступные планировщики
    uint8_t resource_elements;                            // Количество доступных OFDMA-символов в одном ресурсном блоке (RB)
};

class StandardManager
{
public:
    // Статическая функция для получения информации о стандарте
    static StandardInfo get_standard_info(const std::string &standard_name);

    // Статическая функция для получения TTI по строковому ключу
    static double get_tti(
        const std::string &standard_name,
        const std::string &tti_name);

    // Статическая функция для получения схемы модуляции по строковому ключу
    static uint8_t get_modulation_scheme(
        const std::string &standard_name,
        const std::string &modulation_scheme_name);

    // Статическая функция для получения количества ресурсных элементов по строковому ключу
    static uint8_t get_resource_elements_in_resource_block(
        const std::string &standard_name);

    static int get_resource_block_effective_data_size(
        const std::string &standard_name,
        const std::string &modulation_scheme_name);

    static double get_channel_sync_interval(
        const std::string &standard_name,
        const std::string &channel_sync_interval_name);

    // Статическая функция для инициализации данных
    static void initialize();

private:
    // Информации о стандартах
    static std::map<std::string, StandardInfo> standard_info_map;
};
