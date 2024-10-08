#include <random>
#include "time_generator.hpp"
#include "types.hpp"

std::mt19937 TimeGenerator::gen;
ms TimeGenerator::initial_time;
std::uniform_int_distribution<> TimeGenerator::distrib(0, 10);

void TimeGenerator::initialize()
{
    std::random_device rd; // a seed source for the random number engine
    gen.seed(rd());  // mersenne_twister_engine seeded with rd()

    time_point current_time = SystemClock::now();
    initial_time =
        std::chrono::duration_cast<ms>(current_time.time_since_epoch());
}

ms TimeGenerator::generate_time()
{
    ms time_shift = ms(distrib(gen));
    ms result_time = initial_time + time_shift;
    return result_time;
}

ms TimeGenerator::get_initial_time()
{
    return initial_time;
}

std::uniform_int_distribution<> TimeGenerator::get_distribution()
{
    return distrib;
}