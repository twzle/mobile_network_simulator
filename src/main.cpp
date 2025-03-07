#include "core/time_generator.hpp"
#include "core/user_generator.hpp"
#include "core/executor.hpp"
#include "utils/plotter.hpp"
#include "standard_info.hpp"
#include "settings.hpp"
#include "user_config.hpp"
#include "bs_config.hpp"
#include <stdint.h>

int main()
{
    TimeGenerator::initialize();
    UserGenerator::initialize();

    User::initialize();
    StandardManager::initialize();

    int launches = 1; // Количество перезапусков

    std::string standard_type = "LTE";                 // Стандарт связи
    std::string scheduler_type = "DefaultPFScheduler"; // Тип планировщика
    std::string area_type = "Dense Urban";             // Тип среды

    std::string tti_duration = "1ms";           // Длительность TTI
    std::string channel_sync_interval = "10ms"; // Интервал синхронизации канала

    double carrier_frequency = 2000; // Несущая частота в МГц
    int bs_transmission_power = 46;  // Мощность предачи данных на базовой станции в дБ

    double bandwidth = 5; // Полоса пропускания в МГц
    uint8_t base_cqi = 1; // СQI (1-15)

    int packet_count = 10;     // Количество пакетов в очереди
    int packet_size = 159;     // Размер пакета в байтах
    double time_lambda = 1500; // Частота (количество) прихода пакетов в отедельную очередь за секунду (1/с), среднее время между приходом пакетов (1/lambda)

    int queue_count = 1;         // Количество очередей
    double queue_quant = 100;    // Квант времени (RB)
    int queue_limit = 10000;     // Размер очереди
    int users_per_tti_limit = 4; // Лимит обслуживания пользователей за TTI (PUCCH)

    BSConfig bs_config = {0, 0, 25}; // Базовая станция

    std::vector<UserConfig> user_configs = {
        // Пользователи
        {8000, 8000, 1.5, 0, "random", 10},
    };
    int throughput_history_size = 2000;

    Settings settings =
        Settings(
            launches,
            standard_type,
            base_cqi,
            tti_duration,
            channel_sync_interval,
            scheduler_type,
            bandwidth,
            packet_count,
            packet_size,
            queue_count,
            queue_quant,
            queue_limit,
            time_lambda,
            user_configs,
            bs_config,
            carrier_frequency,
            bs_transmission_power,
            area_type,
            users_per_tti_limit,
            throughput_history_size
        );

    try
    {
        settings.validate();
        settings.validate_scheduler_specific_parameters();
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << "Invalid settings: " << e.what() << std::endl;
        return 1;
    }

    StandardManager::set_current_standard(standard_type);

    Executor executor = Executor(settings);
    executor.run();

    MeanStats &stats = executor.get_stats();
    stats.calculate();
    stats.show();
    stats.evaluate_confidence_intervals();

    // std::string stats_file_path = stats.write_yaml();

    // Plotter plotter = Plotter();
    // plotter.run(stats_file_path);
    return 0;
}