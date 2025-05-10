#include "config/standard_info.hpp"
#include <iostream>

// Статическое объявление мап
std::map<std::string, StandardInfo> StandardManager::standard_info_map;

// Статическое объявление текущего названия стандарта
std::string StandardManager::current_standard_name = "LTE";
// Статическое объявление текущего названия стандарта
double StandardManager::current_bandwidth = 20;

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

// Статическая функция для получения информации о стандарте
void StandardManager::set_bandwidth(const double bandwidth)
{
    try
    {
        StandardInfo info = get_standard_info(current_standard_name);
        info.bandwidth_to_rb.at(bandwidth);
    }
    catch (std::exception &ex)
    {
        throw(ex);
    }

    current_bandwidth = bandwidth;
}

// Статическая функция для получения информации о стандарте
double StandardManager::get_bandwidth()
{
    return current_bandwidth;
}

// Статическая функция для получения TTI по строковому ключу
double StandardManager::get_tti(
    const std::string &tti_name)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    return standard_info.ttis.at(tti_name);
}

// Статическая функция для получения размера полезных данных (бит) в RE для CQI (1-15)
double StandardManager::get_cqi_efficiency(
    const uint8_t cqi)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);

    std::tuple<std::string, double, double, int> mcs = standard_info.cqi_to_mcs.at(cqi);

    double spectral_efficiency = std::get<2>(mcs);
    return spectral_efficiency;
}

// Статическая функция для получения количества RB в канале
int StandardManager::get_rb_number_from_bandwidth(
    const double bandwidth)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);

    auto it = standard_info.bandwidth_to_rb.lower_bound(bandwidth);

    if (it != standard_info.bandwidth_to_rb.end() &&
        std::fabs(it->first - bandwidth) < epsilon)
    {
        return it->second;
    }
    else
    {
        throw std::out_of_range("Invalid LTE bandwidth.");
    }
}

// Статическая функция для получения CQI по SINR
int StandardManager::get_cqi_from_sinr(
    const double sinr)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    int cqi = 1;

    auto it = standard_info.sinr_to_cqi.lower_bound(sinr);

    if (it == standard_info.sinr_to_cqi.end())
    {
        cqi = std::prev(it)->second; // Если SINR больше всех ключей, берём максимальный
    }
    else if (it == standard_info.sinr_to_cqi.begin())
    {
        cqi = it->second; // Если SINR меньше всех ключей, берём минимальный
    }
    else
    {
        cqi = it->second;
    }

    return cqi;
}

// Статическая функция для получения CQI по SINR
std::tuple<std::string, double, double, int> StandardManager::get_mcs_from_cqi(
    const int cqi)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);

    auto mcs = standard_info.cqi_to_mcs[cqi];

    return mcs;
}

// Статическая функция для получения CQI по SINR
int StandardManager::get_tbs_from_mcs(
    const int imcs)
{
    StandardInfo standard_info = get_standard_info(current_standard_name);

    int itbs = standard_info.mcs_to_tbs[imcs];

    return itbs;
}

int StandardManager::get_resource_elements_in_resource_block()
{
    StandardInfo standard_info = get_standard_info(current_standard_name);
    return (int)standard_info.resource_elements;
}

int StandardManager::get_resource_block_effective_data_size(
    const uint8_t cqi)
{

    std::tuple<std::string, double, double, int> mcs =
        StandardManager::get_mcs_from_cqi(cqi);

    int imcs = std::get<3>(mcs);
    int itbs = StandardManager::get_tbs_from_mcs(imcs);

    int bytes = TBS::get_size_for_rb(itbs, 1);

    return bytes;
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
                {// 3GPP Table 7.2.3-1
                 {1, {"QPSK", 0.0762, 0.1523, 5}},
                 {2, {"QPSK", 0.1172, 0.2344, 6}},
                 {3, {"QPSK", 0.1885, 0.3770, 8}},
                 {4, {"QPSK", 0.3008, 0.6016, 9}},
                 {5, {"QPSK", 0.4385, 0.8770, 10}},
                 {6, {"QPSK", 0.5879, 1.1758, 12}},
                 {7, {"16-QAM", 0.3691, 1.4766, 13}},
                 {8, {"16-QAM", 0.4785, 1.9141, 14}},
                 {9, {"16-QAM", 0.6016, 2.4063, 16}},
                 {10, {"64-QAM", 0.4551, 2.7305, 17}},
                 {11, {"64-QAM", 0.5537, 3.3223, 19}},
                 {12, {"64-QAM", 0.6504, 3.9023, 20}},
                 {13, {"64-QAM", 0.7539, 4.5234, 21}},
                 {14, {"64-QAM", 0.8525, 5.1152, 23}},
                 {15, {"64-QAM", 0.9258, 5.5547, 24}}},
                {// 3GPP Table 7.1.7.1-1
                 {0, 0},
                 {1, 1},
                 {2, 2},
                 {3, 3},
                 {4, 4},
                 {5, 5},
                 {6, 6},
                 {7, 7},
                 {8, 8},
                 {9, 9},
                 {10, 9},
                 {11, 10},
                 {12, 11},
                 {13, 12},
                 {14, 13},
                 {15, 14},
                 {16, 15},
                 {17, 15},
                 {18, 16},
                 {19, 17},
                 {20, 18},
                 {21, 19},
                 {22, 20},
                 {23, 21},
                 {24, 22},
                 {25, 23},
                 {26, 24},
                 {27, 25},
                 {28, 26}},
                {{"QPSK", 2}, {"16-QAM", 4}, {"64-QAM", 8}},
                {{-6.9390, 1}, {-5.1470, 2}, {-3.1800, 3}, {-1.2530, 4}, {0.7610, 5}, {2.6990, 6}, {4.6930, 7}, {6.5250, 8}, {8.5730, 9}, {10.3660, 10}, {12.2890, 11}, {14.1730, 12}, {15.8880, 13}, {17.8140, 14}, {19.8290, 15}},
                {{1.4, 6}, {3, 15}, {5, 25}, {10, 50}, {15, 75}, {20, 100}},
                {"DefaultRRScheduler",
                 "FixedDRRScheduler", "FixedDRRSchedulerWithUserQuant",
                 "CyclicDRRScheduler", "CyclicDRRSchedulerWithUserQuant",
                 "DefaultDRRScheduler", "DefaultDRRSchedulerWithUserQuant",
                 "DefaultPFScheduler"},
                {{0, "random"}, {1, "forward"}, {2, "backward"}, {3, "left"}, {4, "right"}},
                {"Dense Urban", "Urban", "Suburban"},
                {4, 8},     // Лимит пользователей обслуживаемых за TTI
                12 * 7 * 2, // Число RE (12 поднесущих * 7 OFDMA-символов * 2 слота в субкадре)
            },              // 168 * 0.1523 * 100 / 8 = 319 байт
        },
    };
}
