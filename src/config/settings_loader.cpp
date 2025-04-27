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

    uint8_t base_cqi = config["base_cqi"].as<uint8_t>();
    double time_lambda = config["time_lambda"].as<double>();
    int users_per_tti_limit = config["users_per_tti_limit"].as<int>();

    // Базовая станция
    BSConfig bs_config = {config["bs_config"]["x"].as<double>(),
                          config["bs_config"]["y"].as<double>(),
                          config["bs_config"]["z"].as<double>(),
                          config["bs_config"]["carrier_frequency"].as<double>(),
                          config["bs_config"]["bandwidth"].as<double>(),
                          config["bs_config"]["transmission_power"].as<int>(),
                        };

    // Загрузка пользователей
    std::vector<QueueConfig> queue_configs;
    for (const auto &queue : config["queue_configs"])
    {
        queue_configs.push_back({queue["packet_count"].as<int>(),
                                queue["quant"].as<double>()});
    }

    // Загрузка пользователей
    std::vector<UserConfig> user_configs;
    for (const auto &user : config["user_configs"])
    {
        user_configs.push_back({user["x"].as<double>(),
                                user["y"].as<double>(),
                                user["z"].as<double>(),
                                user["speed"].as<double>(),
                                user["direction"].as<std::string>(),
                                user["quant"].as<double>(),
                                user["packet_size"].as<int>(),
                                user["traffic_part"].as<double>()});
    }

    int throughput_history_size = config["throughput_history_size"].as<int>();
    int fairness_history_size = config["fairness_history_size"].as<int>();

    return Settings(
        launches, standard_type, base_cqi, tti_duration, channel_sync_interval,
        scheduler_type, queue_configs, user_configs, time_lambda, 
        bs_config, area_type, users_per_tti_limit, 
        throughput_history_size, fairness_history_size);
}