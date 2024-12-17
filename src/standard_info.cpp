#include "standard_info.hpp"

// Статическое объявление мап
std::map<std::string, StandardInfo> StandardManager::standard_info_map;
std::map<std::string, double> StandardManager::tti_map;

// Статическая функция для получения информации о стандарте
StandardInfo StandardManager::get_standard_info(const std::string& standard_name)
{
    return standard_info_map.at(standard_name);
}

// Статическая функция для получения TTI по строковому ключу
double StandardManager::get_tti(const std::string& tti_key)
{
    return tti_map.at(tti_key);
}

// Инициализация данных
void StandardManager::initialize()
{
    standard_info_map = {
        {"LTE", 
            {"LTE", "Long-Term Evolution", 
                {
                    {"1ms", 0.001}
                },
                { 1.4, 3, 5, 10, 15, 20 }, 
                {"FixedDRRScheduler", "CircularDRRScheduler", "DefaultDRRScheduler"}
            }
        },
        {"NR_5G", 
            {"5G NR", "5G New Radio", 
                {
                    {"1ms", 0.001}, {"2ms", 0.002}
                }, 
                {10, 20, 50}, 
                {"FixedDRRScheduler", "DefaultDRRScheduler"}
            }
        }
    };

    tti_map = {
        {"1ms", 0.001},
        {"2ms", 0.002},
        {"0.5ms", 0.0005}
    };
}
