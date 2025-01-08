#include "core/user_generator.hpp"

std::mt19937 UserGenerator::gen;
std::uniform_int_distribution<> UserGenerator::distrib;

void UserGenerator::initialize()
{
    std::random_device rd; // зерно для рандомизации
    gen.seed(rd());  // mersenne_twister_engine с зерном rd()
}

int UserGenerator::generate_user_id()
{
    int user_id = distrib(gen);
    return user_id;
}

std::uniform_int_distribution<> UserGenerator::get_distribution()
{
    return distrib;
}

void UserGenerator::set_distribution(std::uniform_int_distribution<> new_distrib)
{
    distrib = new_distrib;
}