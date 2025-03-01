#include "standard_info.hpp"
#include <iostream>

// Статическое объявление мап
std::map<std::string, StandardInfo> StandardManager::standard_info_map;

// Статическое объявление текущего названия стандарта
std::string StandardManager::current_standard_name = "LTE";

// Статическая функция для получения информации о стандарте
void StandardManager::set_current_standard(const std::string &standard_name)
{
    try
    {
        get_standard_info(standard_name);
    }
    catch (std::exception &ex)
    {
        throw(ex);
    }

    current_standard_name = standard_name;
}

// Статическая функция для получения информации о стандарте
StandardInfo StandardManager::get_standard_info(const std::string &standard_name)
{
    return standard_info_map.at(standard_name);
}

// Статическая функция для получения TTI по строковому ключу
double StandardManager::get_tti(
    const std::string &tti_name)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    return standard_info.ttis.at(tti_name);
}

// Статическая функция для получения размера полезных данных (бит) для CQI (1-15)
double StandardManager::get_cqi_efficiency(
    const uint8_t &cqi)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    return standard_info.cqi_efficiency.at(cqi);
}

// Статическая функция для получения размера полезных данных (бит) для CQI (1-15)
int StandardManager::get_cqi_from_sinr(
    const double sinr)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    int cqi = 0;

    auto it = standard_info.sinr_to_cqi.lower_bound(sinr);

    if (it == standard_info.sinr_to_cqi.end())
    {
        cqi = std::prev(it)->second; // Если SINR больше всех ключей, берём максимальный
    }
    if (it == standard_info.sinr_to_cqi.begin())
    {
        cqi = it->second; // Если SINR меньше всех ключей, берём минимальный
    }

    return cqi;
}

uint8_t StandardManager::get_resource_elements_in_resource_block()
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    return standard_info.resource_elements;
}

int StandardManager::get_resource_block_effective_data_size(
    const uint8_t &cqi)
{
    const double effective_bits_per_resource_element =
        static_cast<int>(
            StandardManager::get_cqi_efficiency(
                cqi));

    const int resource_elements_per_resource_block =
        static_cast<int>(
            StandardManager::get_resource_elements_in_resource_block());

    // Используем int для предотвращения переполнения
    const int effective_bits_per_resource_block =
        effective_bits_per_resource_element * resource_elements_per_resource_block;

    // Округление вниз — деление на 8 отбрасывает остаток
    return effective_bits_per_resource_block / 8;
}

double StandardManager::get_channel_sync_interval(
    const std::string &channel_sync_interval_name)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    return standard_info.channel_sync_intervals.at(channel_sync_interval_name);
}

std::string StandardManager::get_mobility_direction(
    const uint8_t mobility_direction_id)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    return standard_info.mobility_directions.at(mobility_direction_id);
}

// Инициализация данных
void StandardManager::initialize()
{
    standard_info_map = {
        {
            "LTE",
            {
                "LTE",
                "Long-Term Evolution",
                {{"1ms", 0.001}},
                {{"10ms", 0.010}},
                {
                    {1, 0.1523}, {2, 0.2344}, {3, 0.3770}, {4, 0.6016}, {5, 0.8770}, {6, 1.1758}, {7, 1.4766}, {8, 1.9141}, {9, 2.4063}, {10, 2.7305}, {11, 3.3223}, {12, 3.9023}, {13, 4.5234}, {14, 5.1152}, {15, 5.5547} // 3GPP Table 7.2.3-1
                },
                {{-6.9390, 1}, {-5.1470, 2}, {-3.1800, 3}, {-1.2530, 4}, {0.7610, 5}, {2.6990, 6}, {4.6930, 7}, {6.5250, 8}, {8.5730, 9}, {10.3660, 10}, {12.2890, 11}, {14.1730, 12}, {15.8880, 13}, {17.8140, 14}, {19.8290, 15}},
                {1.4, 3, 5, 10, 15, 20},
                {"DefaultRRScheduler",
                 "FixedDRRScheduler",
                 "CyclicDRRScheduler",
                 "DefaultDRRScheduler"},
                {{0, "random"}, {1, "forward"}, {2, "backward"}, {3, "left"}, {4, "right"}},
                {"Urban", "Suburban", "Rural"},
                12 * 7 * 2, // Число RE (12 поднесущих * 7 OFDMA-символов * 2 слота в субкадре)
            },
        },
    };
}
