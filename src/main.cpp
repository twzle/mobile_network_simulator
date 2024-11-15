#include "packet_queue_scheduler.hpp"
#include "time_generator.hpp"
#include "executor.hpp"
#include "plotter.hpp"
#include <random>


int main(){
    TimeGenerator::initialize();

    int launches = 10;
    int packet_count = 1000;
    int packet_size = 1;
    int queue_count = 2;
    int queue_quant = 1;
    int queue_limit = 1000;
    double time_lambda = 2;

    Settings settings = 
    Settings(
        launches, 
        packet_count, 
        packet_size, 
        queue_count, 
        queue_quant, 
        queue_limit,
        time_lambda);
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