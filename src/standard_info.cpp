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

uint8_t StandardManager::get_resource_elements_in_resource_block(
    const std::string &standard_name)
{
    StandardInfo standard_info = get_standard_info(standard_name);
    return standard_info.resource_elements;
}

int StandardManager::get_resource_block_effective_data_size(
    const std::string &standard_name,
    const std::string &modulation_scheme_name)
{
    const int effective_bits_per_resource_element =
        static_cast<int>(
            StandardManager::get_modulation_scheme(
                standard_name,
                modulation_scheme_name));

    const int resource_elements_per_resource_block =
        static_cast<int>(
            StandardManager::get_resource_elements_in_resource_block(
                standard_name));

    // Используем int для предотвращения переполнения
    const int effective_bits_per_resource_block =
        effective_bits_per_resource_element * resource_elements_per_resource_block;

    // Округление вниз — деление на 8 отбрасывает остаток
    return effective_bits_per_resource_block / 8;
}

double StandardManager::get_channel_sync_interval(
    const std::string &standard_name,
    const std::string &channel_sync_interval_name)
{
    StandardInfo standard_info = get_standard_info(standard_name);
    return standard_info.channel_sync_intervals.at(channel_sync_interval_name);
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
                {{"5ms", 0.005}, {"10ms", 0.010}}, 
                {{"Dynamic", 0}, {"QPSK", 2}, {"16-QAM", 4}, {"64-QAM", 8}, {"256-QAM", 16}}, 
                {1.4, 3, 5, 10, 15, 20}, 
                {
                    "DefaultRRScheduler", 
                    "FixedDRRScheduler", 
                    "CyclicDRRScheduler", 
                    "DefaultDRRScheduler"
                },
                84, // 12 поднесущих * 7 OFDMA-символов
            },
        },
        {
            "NR_5G",
            {
                "5G NR", 
                "5G New Radio", 
                {{"1ms", 0.001}, {"2ms", 0.002}, {"0.5ms", 0.0005}}, 
                {{"5ms", 0.005}, {"10ms", 0.010}}, 
                {{"Dynamic", 0}, {"QPSK", 2}, {"16-QAM", 4}, {"64-QAM", 8}, {"256-QAM", 16}}, 
                {10, 20, 50}, 
                {
                    "DefaultRRScheduler", 
                    "FixedDRRScheduler", 
                    "CyclicDRRScheduler", 
                    "DefaultDRRScheduler"
                },
                84, // 12 поднесущих * 7 OFDMA-символов
            },
        }};
}
