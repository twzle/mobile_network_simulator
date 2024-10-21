#include "packet_queue_scheduler.hpp"
#include "time_generator.hpp"
#include "executor.hpp"
#include <random>

// Равномерное распределение в диапазоне [a, b] (мс)
std::uniform_int_distribution<> TimeGenerator::distrib(0, 10);

int main(){
    TimeGenerator::initialize();

    int launches = 10;
    int packet_count = 500;
    int packet_size = 2;
    int queue_count = 2;
    int queue_quant = 1;
    int queue_limit = 1000;

    Settings settings = 
    Settings(
        launches, 
        packet_count, 
        packet_size, 
        queue_count, 
        queue_quant, 
        queue_limit);
    Executor executor = Executor(settings);
    executor.run();

    AverageStats stats = executor.get_stats();
    stats.calculate();
    stats.show();
    stats.draw_delay_plot();
    stats.draw_scheduling_plot();
    
    return 0;
}