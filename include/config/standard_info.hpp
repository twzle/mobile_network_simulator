#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <tuple>
#include <stdint.h>
#include "const.hpp"
#include "config/tbs.hpp"

struct StandardInfo
{
    // Название стандарта
    std::string name;
    // Краткое описание
    std::string description;
    // Доступные интервалы TTI (строковое обозначение к числу секунд)
    std::map<std::string, double> ttis;
    // Доступные интервалы синхронизации канала (строковое обозначение к числу секунд)
    std::map<std::string, double> channel_sync_intervals;
    // CQI (Номер -> модуляция, code rate, спектральная эффективность, IMCS)
    std::map<uint8_t, std::tuple<std::string, double, double, int> > cqi_to_mcs;
    // MCS to TBS (IMCS -> ITBS)
    std::map<int, int> mcs_to_tbs;
    // Схемы модуляции канала (название -> бит/RE)
    std::map<std::string, int> modulation_schemes;
    // SINR to CQI
    std::map<double, uint8_t> sinr_to_cqi;
    // Доступные полосы пропускания (в МГц) к максимальному числу RB в них
    std::map<double, int> bandwidth_to_rb;
    // Доступные планировщики
    std::vector<std::string> schedulers;
    // Направления перемещения пользователя
    std::map<uint8_t, std::string> mobility_directions;
    // Типы местности
    std::vector<std::string> area_types;
    // Лимит пользователей обслуживаемых за TTI
    std::vector<int> users_per_tti_limits;
    // Количество доступных ресурсных элементов (RE) в одном ресурсном блоке (RB)
    uint8_t resource_elements;
};

class StandardManager
{
public:
    // Статическая функция для получения информации о стандарте
    static StandardInfo get_standard_info(const std::string &standard_name);
    static void set_current_standard(const std::string &standard_name);

    // Статическая функция для получения информации о стандарте
    static double get_bandwidth();
    static void set_bandwidth(const double bandwidth);

    // Статическая функция для получения TTI по строковому ключу
    static double get_tti(
        const std::string &tti_name);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static double get_cqi_efficiency(
        const uint8_t cqi);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static int get_rb_number_from_bandwidth(
        const double bandwidth);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static int get_cqi_from_sinr(
        const double sinr);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static std::tuple<std::string, double, double, int> get_mcs_from_cqi(const int cqi);

    // Статическая функция для получения числа полезных в RE по строковому ключу
    static int get_tbs_from_mcs(const int imcs);

    // Статическая функция для получения количества ресурсных элементов по строковому ключу
    static int get_resource_elements_in_resource_block();

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
    static double current_bandwidth;
};
