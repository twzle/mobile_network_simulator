#include "time_generator.hpp"
#include "executor.hpp"
#include "plotter.hpp"
#include "settings.hpp"
#include <iostream>
#include <random>

int main(){
    TimeGenerator::initialize();

    int launches = 1; // Количество перезапусков
    double bandwidth = 10; // Полоса пропускания в МГц
    int scheduler_type = 3;
    int packet_count = 10000; // Количество пакетов в очереди
    int packet_size = 10; // Размер пакета (RB)
    int queue_count = 4; // Количество очередей
    int queue_quant = 1; // Квант времени (RB)
    int queue_limit = 10000; // Размер очереди
    double time_lambda = 200; // Частота прихода пакетов

    Settings settings = 
        Settings(
            launches, 
            bandwidth,
            scheduler_type,
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