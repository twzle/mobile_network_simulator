#include "scenarios/scenarios.hpp"

namespace BasicScenario {
    Settings create_settings() {
        int launches = 1;
        std::string standard_type = "LTE";
        std::string scheduler_type = "DefaultPFScheduler";
        std::string area_type = "Dense Urban";
    
        std::string tti_duration = "1ms";
        std::string channel_sync_interval = "10ms";
    
        uint8_t base_cqi = 1;
        double time_lambda = 10000;
        int users_per_tti_limit = 8;

        double carrier_frequency = 2000;
        double bandwidth = 5;
        int transmission_power = 46;
    
        BSConfig bs_config = {
            0, 0, 25, 
            carrier_frequency, bandwidth, transmission_power
        };

        std::vector<QueueConfig> queue_configs = {
            {2000, 10}
        };
    
        std::vector<UserConfig> user_configs = {
            {1000, 1000, 1.5, 0, "random", 10, 150, 0.2},
            {6500, 6500, 1.5, 0, "random", 10, 150, 0.2},
            {8000, 8000, 1.5, 0, "random", 10, 150, 0.2},
            {10000, 10000, 1.5, 0, "random", 10, 150, 0.2},
            {11000, 11000, 1.5, 0, "random", 10, 150, 0.2},
        };

        // std::vector<UserConfig> user_configs = {
        //     {1000, 1000, 1.5, 0, "random", 10, 150, 0.2},
        //     {1000, 1000, 1.5, 0, "random", 10, 150, 0.2},
        //     {1000, 1000, 1.5, 0, "random", 10, 150, 0.2},
        //     {1000, 1000, 1.5, 0, "random", 10, 150, 0.2},
        //     {1000, 1000, 1.5, 0, "random", 10, 150, 0.2},
        // };

        int throughput_history_size = 200;
        int fairness_history_size = 200;
    
        return Settings(
            launches, standard_type, base_cqi, tti_duration, channel_sync_interval,
            scheduler_type, queue_configs, user_configs, time_lambda, 
            bs_config, area_type, users_per_tti_limit, 
            throughput_history_size, fairness_history_size);
    }
}
