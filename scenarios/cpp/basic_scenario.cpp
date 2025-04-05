#include "scenarios/scenarios.hpp"

namespace BasicScenario {
    Settings create_settings() {
        int launches = 10;
        std::string standard_type = "LTE";
        std::string scheduler_type = "DefaultPFScheduler";
        std::string area_type = "Dense Urban";
    
        std::string tti_duration = "1ms";
        std::string channel_sync_interval = "10ms";
    
        double carrier_frequency = 2000;
        int bs_transmission_power = 46;
    
        double bandwidth = 5;
        uint8_t base_cqi = 1;
    
        int packet_count = 1000;
        int packet_size = 150;
        double time_lambda = 1000;
    
        int queue_count = 1;
        double queue_quant = 100;
        int queue_limit = 10000;
        int users_per_tti_limit = 4;
    
        BSConfig bs_config = {0, 0, 25};
    
        std::vector<UserConfig> user_configs = {
            {1000, 1000, 1.5, 0, "random", 10},
            {6500, 6500, 1.5, 0, "random", 10},
            {8000, 8000, 1.5, 0, "random", 10},
            {10000, 10000, 1.5, 0, "random", 10},
            {11000, 11000, 1.5, 0, "random", 10},
        };
        int throughput_history_size = 200;
    
        return Settings(
            launches, standard_type, base_cqi, tti_duration, channel_sync_interval,
            scheduler_type, bandwidth, packet_count, packet_size, queue_count, queue_quant,
            queue_limit, time_lambda, user_configs, bs_config, carrier_frequency,
            bs_transmission_power, area_type, users_per_tti_limit, throughput_history_size
        );
    }
}
