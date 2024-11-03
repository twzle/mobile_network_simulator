#include "packet_queue_scheduler.hpp"
#include "time_generator.hpp"
#include "executor.hpp"
#include <random>

int main(){
    TimeGenerator::initialize();

    int launches = 50;
    int packet_count = 1000;
    int packet_size = 1;
    int queue_count = 4;
    int queue_quant = 1;
    int queue_limit = 5000;
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

    AverageStats stats = executor.get_stats();
    stats.calculate();
    stats.show();
    stats.draw_delay_plot();
    
    return 0;
}