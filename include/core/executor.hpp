#pragma once

#include "config/settings.hpp"
#include "stats/mean_stats.hpp"

class Executor
{
    public:
        Executor(Settings settings);
        std::unique_ptr<BaseScheduler> initialize_scheduler();
        void run();
        void execute();
        MeanStats& get_stats();
    
    private:
        Settings settings; // Конфигурация запуска
        MeanStats mean_stats; // Статистика по всем запускам планировщика
};