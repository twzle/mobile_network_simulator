#include "standard_info.hpp"
#include <iostream>

// Статическое объявление мап
std::map<std::string, StandardInfo> StandardManager::standard_info_map;

// Статическая функция для получения информации о стандарте
StandardInfo StandardManager::get_standard_info(const std::string &standard_name)
{
    return standard_info_map.at(standard_name);
}

// Статическая функция для получения TTI по строковому ключу
double StandardManager::get_tti(
    const std::string &standard_name,
    const std::string &tti_name)
{
    StandardInfo standard_info = get_standard_info(standard_name);
    return standard_info.ttis.at(tti_name);
}

// Статическая функция для получения TTI по строковому ключу
uint8_t StandardManager::get_modulation_scheme(
    const std::string &standard_name,
    const std::string &modulation_scheme_name)
{
    StandardInfo standard_info = get_standard_info(standard_name);
    return standard_info.modulation_schemes.at(modulation_scheme_name);
}

uint8_t StandardManager::get_resource_elements(
    const std::string &standard_name)
{
    StandardInfo standard_info = get_standard_info(standard_name);
    return standard_info.resource_elements;
}

// Инициализация данных
void StandardManager::initialize()
{
    standard_info_map = {
        {"LTE",
            {"LTE", "Long-Term Evolution", 
                {{"1ms", 0.001}}, 
                {{"Dynamic", 0},{"QPSK", 2},{"16-QAM", 4},{"64-QAM", 8},{"256-QAM", 16}},
                {1.4, 3, 5, 10, 15, 20},
                {"FixedDRRScheduler", "CircularDRRScheduler", "DefaultDRRScheduler"},
                84, // 12 поднесущих * 7 OFDMA-символов
            },
        },
        {"NR_5G", 
            {"5G NR", "5G New Radio", 
                {{"1ms", 0.001}, {"2ms", 0.002}, {"0.5ms", 0.0005}}, 
                {{"Dynamic", 0}, {"QPSK", 2}, {"16-QAM", 4}, {"64-QAM", 8}, {"256-QAM", 16}}, 
                {10, 20, 50}, 
                {"FixedDRRScheduler", "DefaultDRRScheduler"},
                84, // 12 поднесущих * 7 OFDMA-символов 
            },
        }
    };
}
