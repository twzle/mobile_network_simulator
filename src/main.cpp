#include "core/time_generator.hpp"
#include "core/user_generator.hpp"
#include "core/executor.hpp"
#include "utils/plotter.hpp"
#include "config/standard_info.hpp"
#include "config/settings.hpp"
#include "scenarios/scenarios.hpp"
#include "config/user_config.hpp"
#include "config/bs_config.hpp"
#include "config/tbs.hpp"
#include <stdint.h>

// Место для указания используемого сценария
Settings use_scenario() {
    return BasicScenario::create_settings();
}

Settings initialize_settings(int argc, char* argv[]) {
    Settings settings;

    if (argc > 1) {
        std::string config_file = argv[1];
        try {
            settings = load_settings_from_yaml(config_file);
            std::cout << "Loaded settings from " << config_file << "\n\n";
        } catch (const std::exception& e) {
            std::cerr << "Failed to load YAML settings: " << e.what() << "\n";
            exit(1);
        }
    } else {
        settings = use_scenario();
        std::cout << "Using default settings" << "\n\n";
    }
    return settings;
}


int main(int argc, char* argv[])
{
    TBS::initialize();

    TimeGenerator::initialize();
    UserGenerator::initialize();

    User::initialize();
    StandardManager::initialize();

    Settings settings = initialize_settings(argc, argv);
    StandardManager::set_current_standard(settings.get_standard_type());

    try
    {
        settings.validate();
        settings.validate_scheduler_specific_parameters();
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << "Invalid settings: " << e.what() << std::endl;
        return 1;
    }

    Executor executor = Executor(settings);
    executor.run();

    MeanStats &stats = executor.get_stats();
    stats.calculate(settings.get_bandwidth());
    stats.show();
    stats.evaluate_confidence_intervals();

    std::string stats_file_path = stats.write_yaml();

    Plotter plotter = Plotter();
    plotter.run(stats_file_path);
    return 0;
}