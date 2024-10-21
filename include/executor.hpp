#pragma once

#include "settings.hpp"
#include "average_stats.hpp"

class Executor
{
    public:
        Executor(Settings settings);
        void run();
        void execute();
        AverageStats get_stats();
    
    private:
        Settings settings; // Конфигурация запуска
        AverageStats average_stats; // Статистика по всем запускам планировщика
};