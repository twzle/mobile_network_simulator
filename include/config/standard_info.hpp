#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <stdint.h>
#include "const.hpp"

struct StandardInfo
{
    std::string name;                                              // Название стандарта
    std::string description;                                       // Краткое описание
    std::map<std::string, double> ttis;                            // Доступные интервалы TTI (строковое обозначение к числу секунд)
    std::map<std::string, double> channel_sync_intervals;          // Доступные интервалы синхронизации канала (строковое обозначение к числу секунд)
    std::map<uint8_t, std::pair<std::string, double> > cqi_to_mcs; // CQI (строковое обозначение к числу бит полезных данных в RE)
    std::map<std::string, int> modulation_schemes;                 // Схемы модуляции канала (название -> бит/RE)
    std::map<double, uint8_t> sinr_to_cqi;                         // SINR to CQI
    std::map<double, int> bandwidth_to_rb;                         // Доступные полосы пропускания (в МГц) к максимальному числу RB в них
    std::vector<std::string> schedulers;                           // Доступные планировщики
    std::map<uint8_t, std::string> mobility_directions;            // Направления перемещения пользователя
    std::vector<std::string> area_types;                           // Типы местности
    std::vector<int> users_per_tti_limits;                         // Лимит пользователей обслуживаемых за TTI
    uint8_t resource_elements;                                     // Количество доступных ресурсных элементов (RE) в одном ресурсном блоке (RB)
};

class StandardManager
{
public:
    // Статическая функция для получения информации о стандарте
    static StandardInfo get_standard_info(const std::string &standard_name);
    static void set_current_standard(const std::string &standard_name);

    // Статическая функция для получения TTI по строковому ключу
    static double get_tti(
        const std::string &tti_name);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static double get_cqi_efficiency(
        const uint8_t cqi);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static double get_rb_number_from_bandwidth(
        const double bandwidth);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static int get_cqi_from_sinr(
        const double cqi);

    // Статическая функция для получения количества ресурсных элементов по строковому ключу
    static uint8_t get_resource_elements_in_resource_block();

    static int get_resource_block_effective_data_size(
        const uint8_t cqi);

    static double get_channel_sync_interval(
        const std::string &channel_sync_interval_name);

    static std::string get_mobility_direction(
        const uint8_t mobility_direction_id);

    // Статическая функция для инициализации данных
    static void initialize();

private:
    // Информации о стандартах
    static std::map<std::string, StandardInfo> standard_info_map;
    static std::string current_standard_name;
};
