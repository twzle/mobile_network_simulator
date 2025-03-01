#include "core/user_generator.hpp"

std::mt19937 UserGenerator::user_id_gen;
std::mt19937 UserGenerator::user_move_direction_gen;

std::uniform_int_distribution<> UserGenerator::user_id_distrib;
std::uniform_int_distribution<> UserGenerator::user_move_direction_distrib;

void UserGenerator::initialize()
{
    std::random_device user_id_rd; // зерно для рандомизации
    user_id_gen.seed(user_id_rd());  // mersenne_twister_engine с зерном rd()

    std::random_device user_move_direction_rd; // зерно для рандомизации
    user_move_direction_gen.seed(user_move_direction_rd());  // mersenne_twister_engine с зерном rd()
}

int UserGenerator::generate_user_id()
{
    int user_id = user_id_distrib(user_id_gen);
    return user_id;
}

std::uniform_int_distribution<> UserGenerator::get_user_id_distribution()
{
    return user_id_distrib;
}

void UserGenerator::set_user_id_distribution(std::uniform_int_distribution<> new_distrib)
{
    user_id_distrib = new_distrib;
}

int UserGenerator::generate_user_move_direction(){
    int user_move_direction = user_move_direction_distrib(user_move_direction_gen);
    return user_move_direction;
}

std::uniform_int_distribution<> UserGenerator::get_user_move_direction_distribution(){
    return user_move_direction_distrib;
}

void UserGenerator::set_user_move_direction_distribution(std::uniform_int_distribution<> new_distrib){
    user_move_direction_distrib = new_distrib;
}
