#include "core/time_generator.hpp"
#include "core/executor.hpp"
#include "utils/plotter.hpp"
#include "standard_info.hpp" 
#include "settings.hpp"
#include <iostream>
#include <random>

int main(){
    TimeGenerator::initialize();
    StandardManager::initialize();

    int launches = 2; // Количество перезапусков
    std::string standard_type = "LTE"; // Стандарт связи
    std::string tti_duration = "1ms"; // Длительность TTI
    std::string scheduler_type = "DefaultDRRScheduler";
    double bandwidth = 10; // Полоса пропускания в МГц
    int packet_count = 100; // Количество пакетов в очереди
    int packet_size = 10; // Размер пакета (RB)
    int queue_count = 1; // Количество очередей
    int queue_quant = 1; // Квант времени (RB)
    int queue_limit = 10000; // Размер очереди
    double time_lambda = 2; // Частота прихода пакетов

    Settings settings = 
        Settings(
            launches, 
            standard_type,
            tti_duration,
            scheduler_type,
            bandwidth,
            packet_count, 
            packet_size, 
            queue_count, 
            queue_quant, 
            queue_limit,
            time_lambda
        );

    try {
        settings.validate();
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid settings: " << e.what() << std::endl;
        return 1;
    }

    Executor executor = Executor(settings);
    executor.run();

    AverageStats& stats = executor.get_stats();
    stats.calculate();
    stats.show();

    std::string stats_file_path = stats.write_yaml();


    Plotter plotter = Plotter();
    plotter.run(stats_file_path);
    return 0;
}