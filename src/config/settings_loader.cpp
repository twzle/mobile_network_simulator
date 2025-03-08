#include <yaml-cpp/yaml.h>

#include "config/settings.hpp"

Settings load_settings_from_yaml(const std::string &filename)
{
    std::string scenario_path = "./scenarios/yaml/";
    YAML::Node config = YAML::LoadFile(scenario_path + filename);

    int launches = config["launches"].as<int>();
    std::string standard_type = config["standard_type"].as<std::string>();
    std::string scheduler_type = config["scheduler_type"].as<std::string>();
    std::string area_type = config["area_type"].as<std::string>();

    std::string tti_duration = config["tti_duration"].as<std::string>();
    std::string channel_sync_interval = config["channel_sync_interval"].as<std::string>();

    double carrier_frequency = config["carrier_frequency"].as<double>();
    int bs_transmission_power = config["bs_transmission_power"].as<int>();

    double bandwidth = config["bandwidth"].as<double>();
    uint8_t base_cqi = config["base_cqi"].as<uint8_t>();

    int packet_count = config["packet_count"].as<int>();
    int packet_size = config["packet_size"].as<int>();
    double time_lambda = config["time_lambda"].as<double>();

    int queue_count = config["queue_count"].as<int>();
    double queue_quant = config["queue_quant"].as<double>();
    int queue_limit = config["queue_limit"].as<int>();
    int users_per_tti_limit = config["users_per_tti_limit"].as<int>();

    // Базовая станция
    BSConfig bs_config = {config["bs_config"]["x"].as<double>(),
                          config["bs_config"]["y"].as<double>(),
                          config["bs_config"]["z"].as<double>()};

    // Загрузка пользователей
    std::vector<UserConfig> user_configs;
    for (const auto &user : config["user_configs"])
    {
        user_configs.push_back({user["x"].as<double>(),
                                user["y"].as<double>(),
                                user["z"].as<double>(),
                                user["speed"].as<double>(),
                                user["direction"].as<std::string>(),
                                user["quant"].as<double>()});
    }

    int throughput_history_size = config["throughput_history_size"].as<int>();

    return Settings(
        launches, standard_type, base_cqi, tti_duration, channel_sync_interval,
        scheduler_type, bandwidth, packet_count, packet_size, queue_count, queue_quant,
        queue_limit, time_lambda, user_configs, bs_config, carrier_frequency,
        bs_transmission_power, area_type, users_per_tti_limit, throughput_history_size);
}