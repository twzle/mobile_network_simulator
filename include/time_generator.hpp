#include <random>
#include "types.hpp"

class TimeGenerator {
    public:
        static void initialize();
        static ms generate_time();
        static ms get_initial_time();
        static std::uniform_int_distribution<> get_distribution();

    private:
        static std::mt19937 gen;
        static ms initial_time;
        static std::uniform_int_distribution<> distrib;
};