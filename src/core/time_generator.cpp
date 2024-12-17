#include <random>
#include "core/time_generator.hpp"
#include "types.hpp"

std::mt19937 TimeGenerator::gen;
double TimeGenerator::initial_time;
double TimeGenerator::last_scheduling_time;
std::exponential_distribution<> TimeGenerator::distrib;

void TimeGenerator::initialize()
{
    std::random_device rd; // зерно для рандомизации
    gen.seed(rd());  // mersenne_twister_engine с зерном rd()
    initial_time = 0.0;
    last_scheduling_time = initial_time;
}

void TimeGenerator::reset_time()
{
    last_scheduling_time = initial_time;
}

// Генератор времени относительно создания относительно инициилазации TimeGenerator
double TimeGenerator::generate_time()
{
    double time_delta = distrib(gen);
    double result_time = last_scheduling_time + time_delta;
    last_scheduling_time = result_time;
    return result_time;
}

double TimeGenerator::get_initial_time()
{
    return initial_time;
}

std::exponential_distribution<> TimeGenerator::get_distribution()
{
    return distrib;
}

void TimeGenerator::set_distribution(std::exponential_distribution<> new_distrib)
{
    distrib = new_distrib;
}