#include "core/time_generator.hpp"
#include "core/user_generator.hpp"
#include "core/executor.hpp"
#include "utils/plotter.hpp"
#include "standard_info.hpp"
#include "settings.hpp"

int main()
{
    TimeGenerator::initialize();
    StandardManager::initialize();
    UserGenerator::initialize();
    User::initialize();

    int launches = 10; // Количество перезапусков

    std::string standard_type = "LTE"; // Стандарт связи
    std::string tti_duration = "1ms";  // Длительность TTI
    std::string modulation_scheme = "256-QAM"; // Схема модуляции канала
    std::string scheduler_type = "DefaultDRRScheduler"; // Тип планировщика
    double bandwidth = 20; // Полоса пропускания в МГц

    int packet_count = 1000; // Количество пакетов в очереди
    int packet_size = 18648;    // Размер пакета в байтах

    int queue_count = 4;     // Количество очередей
    double queue_quant = 100;  // Квант времени (RB)
    int queue_limit = 10000; // Размер очереди

    int user_count = 5; // Количество пользователей

    double time_lambda = 5000; // Частота (количество) прихода пакетов в отедельную очередь за секунду (1/с), среднее время между приходом пакетов (1/lambda)

    Settings settings =
        Settings(
            launches,
            standard_type,
            modulation_scheme,
            tti_duration,
            scheduler_type,
            bandwidth,
            packet_count,
            packet_size,
            queue_count,
            queue_quant,
            queue_limit,
            user_count,
            time_lambda);

    try
    {
        settings.validate();
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << "Invalid settings: " << e.what() << std::endl;
        return 1;
    }

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