#include "scenarios/scenarios.hpp"

namespace PF_fairness_1 {
    Settings create_settings() {
        int launches = 10;
        std::string standard_type = "LTE";
        std::string scheduler_type = "DefaultDRRScheduler";
        std::string area_type = "Suburban";
    
        std::string tti_duration = "1ms";
        std::string channel_sync_interval = "10ms";
    
        uint8_t base_cqi = 15;
        int users_per_tti_limit = 8;

        double carrier_frequency = 1800;
        double bandwidth = 20;
        int transmission_power = 46;
    
        BSConfig bs_config = {
            0, 0, 30, 
            carrier_frequency, bandwidth, transmission_power
        };

        std::vector<QueueConfig> queue_configs = {
            {3000, 1000, 15000},
        };
    
        // std::vector<UserConfig> user_configs = {
        //     // {750, 750, 1.5, 0, "random", 11, 500, 1}, // 1 км
        //     {2122, 2122, 1.5, 0, "random", 11, 500, 1}, // 3 км
        //     // {3600, 3600, 1.5, 0, "random", 11, 500, 1}, // 5 км
        //     // {4250, 4250, 1.5, 0, "random", 11, 500, 1}, // 6 км
        //     {4597, 4597, 1.5, 0, "random", 11, 500, 1}, // 6.5 км
        //     {7075, 7075, 1.5, 0, "random", 11, 500, 1}, // 10 км
        // };


        std::vector<UserConfig> user_configs = {
            {750, 750, 1.5, 0, "random", 11, 500, 1},
            {750, 750, 1.5, 0, "random", 11, 500, 1},
            {750, 750, 1.5, 0, "random", 11, 500, 1},
            {750, 750, 1.5, 0, "random", 11, 500, 1},
        };

        int throughput_history_size = 100;
        int fairness_history_size = 2;
    
        return Settings(
            launches, standard_type, base_cqi, tti_duration, channel_sync_interval,
            scheduler_type, queue_configs, user_configs, 
            bs_config, area_type, users_per_tti_limit, 
            throughput_history_size, fairness_history_size);
    }
}
