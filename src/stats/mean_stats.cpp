#include "stats/mean_stats.hpp"

#include <chrono>
#include <thread>

#define DATA_DIR "data/"
#define FILE_NAME "data.yaml"

// Подсчет средних значений
void MeanStats::calculate()
{
    calculate_mean_values();
    calculate_mean_delays();
    calculate_mean_queue_processing_time();

    collect_history();
}

// Сбор исторических данных по запускам для оптимизации доступа к данным
void MeanStats::collect_history()
{
    for (auto &stats : stats_array)
    {
        this->scheduler_total_time_history.push_back(
            stats.scheduler_total_time);
        this->scheduler_processing_time_history.push_back(
            stats.scheduler_processing_time);
        this->scheduler_idle_time_history.push_back(
            stats.scheduler_idle_time);
        this->scheduler_wait_time_history.push_back(
            stats.scheduler_wait_time);

        this->scheduler_fairness_for_queues_history.push_back(
            stats.scheduler_average_fairness_for_queues);

        // for (size_t queue_id = 0; queue_id < stats.queue_average_delay.size(); ++queue_id)
        // {
        //     this->mean_delay_by_queue_history[queue_id]
        //         .push_back(stats.queue_average_delay[queue_id]);
        // }

        // for (size_t queue_id = 0;
        //      queue_id < stats.queue_processing_time.size();
        //      ++queue_id)
        // {
        //     this->mean_processing_time_by_queue_history[queue_id]
        //         .push_back(stats.queue_processing_time[queue_id]);
        // }
    }
}

// Подсчет среднего арифметического базовых параметров работы планировщика
void MeanStats::calculate_mean_values()
{
    for (auto &stats : stats_array)
    {
        common_scheduler_total_time += stats.scheduler_total_time;
        common_scheduler_idle_time += stats.scheduler_idle_time;
        common_scheduler_processing_time += stats.scheduler_processing_time;
        common_scheduler_wait_time += stats.scheduler_wait_time;

        common_scheduler_fairness_for_queues += stats.scheduler_average_fairness_for_queues;

        common_scheduler_packet_count += stats.packet_count;
    }

    mean_scheduler_total_time =
        common_scheduler_total_time / stats_array.size();
    mean_scheduler_idle_time =
        common_scheduler_idle_time / stats_array.size();
    mean_scheduler_processing_time =
        common_scheduler_processing_time / stats_array.size();
    mean_scheduler_wait_time =
        common_scheduler_wait_time / stats_array.size();

    mean_scheduler_fairness_for_queues =
        common_scheduler_fairness_for_queues / stats_array.size();
    
    mean_scheduler_packet_count =
        common_scheduler_packet_count / stats_array.size();
}

// Подсчет среднего арифметического задержек обработки пакетов
void MeanStats::calculate_mean_delays()
{
    int queue_count = stats_array[0].queue_average_delay.size();
    mean_delays_by_queue.resize(queue_count);

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            mean_delays_by_queue[i] += stats.queue_average_delay[i];
        }
        total_mean_delay_by_scheduler += stats.scheduler_average_delay;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        mean_delays_by_queue[i] /= stats_array.size();
    }

    total_mean_delay_by_scheduler /= stats_array.size();
}

void MeanStats::calculate_mean_queue_processing_time()
{
    int queue_count = stats_array[0].queue_total_time.size();
    queue_mean_processing_time.resize(queue_count);

    double total_mean_processing_time = 0;

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            queue_mean_processing_time[i] +=
                stats.queue_total_time[i];
        }
        total_mean_processing_time +=
            stats.queue_average_total_time;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        queue_mean_processing_time[i] /= stats_array.size();
    }

    total_mean_processing_time_by_scheduler = total_mean_processing_time / stats_array.size();
}

/*
Подсчет минимального числа запусков для уровня достоверности
Общая формула: n = (z * S / E)^2,
n - кол-во запусков,
z - коэф. доверия,
S - среднекв. отклонение величины,
E - точность
*/
void MeanStats::calculate_execution_count_for_metric(
    const double &standard_deviation, const double &accuracy)
{
    double credability_out_of_95 = 1.96;  // z
    double credability_out_of_99 = 2.576; // z

    // Корень из количества запусков
    // (z * S / E)
    double product_for_95 = (credability_out_of_95 * standard_deviation / accuracy);
    double product_for_99 = (credability_out_of_99 * standard_deviation / accuracy);

    // Количество запусков
    // (z * S / E)^2
    double launches_for_95 = std::pow(product_for_95, 2);
    double launches_for_99 = std::pow(product_for_99, 2);

    std::cout << "E (допустимая погрешность) = " << accuracy << "\n";
    std::cout << "Количество запусков для достоверности 95% = " 
        << std::ceil(launches_for_95) << " (" << launches_for_95 << ")\n";
    std::cout << "Количество запусков для достоверности 99% = " 
        << std::ceil(launches_for_99) << " (" << launches_for_99 << ")\n";
}

/*
Подсчет стандартного отклонения величины
Общая формула: S = sqrt(sum(Xi - X)^2 / (n - 1)),
S - среднекв. отклонение,
Xi - i-ый элемент выборки,
X - среднее арифметическое выборки,
n - размер выборки
*/
double MeanStats::calculate_standard_deviation_for_metric(
    const std::vector<double> &values, const double &mean)
{
    // sum(Xi - X)^2
    // Сумма квадратов отклонений
    double sum_of_squared_deviations = 0;
    for (auto &value : values)
    {
        // Отклонение от среднего для каждого значения из выборки
        double value_deviation = value - mean;
        sum_of_squared_deviations += value_deviation * value_deviation;
    }

    // sum(Xi - X)^2 / (n - 1)
    // Квадрат среднеквадратического отклонения
    double square_of_standard_deviation =
        sum_of_squared_deviations / (stats_array.size() - 1);

    // sqrt(sum(Xi - X)^2 / (n - 1))
    // Среднеквадратическое отклонение
    double standard_deviation = std::sqrt(square_of_standard_deviation);
    std::cout << "Стандартное отклонение величины = " << standard_deviation << "\n";

    return standard_deviation;
}

void MeanStats::calculate_confidence_interval(
    const std::vector<double> &values, const double &mean, const double &accuracy)
{
    double std_dev = calculate_standard_deviation_for_metric(values, mean);
    calculate_execution_count_for_metric(std_dev, accuracy);
}

void MeanStats::evaluate_confidence_intervals()
{
    std::cout << "\n------------\n";
    std::cout << "Доверительные интервалы" << std::endl;

    // Доверительный интервал для общего времени работы планировщика
    std::cout << "\nОбщее время работы планировщика"
              << " (scheduler_total_time)"
              << std::endl;
    calculate_confidence_interval(
        scheduler_total_time_history,
        mean_scheduler_total_time,
        1);

    // Доверительный интервал для времени работы планировщика в состоянии ожидания
    std::cout << "\nВремя работы планировщика в состоянии ожидания"
              << " (scheduler_wait_time)" << std::endl;
    calculate_confidence_interval(
        scheduler_wait_time_history,
        mean_scheduler_wait_time,
        1);

    // Доверительный интервал для времени работы планировщика в состоянии простоя
    std::cout << "\nВремя работы планировщика в состоянии простоя"
              << " (scheduler_idle_time)" << std::endl;
    calculate_confidence_interval(
        scheduler_idle_time_history,
        mean_scheduler_idle_time,
        1);

    // Доверительный интервал для времени работы планировщика в состоянии обслуживания
    std::cout << "\nВремя работы планировщика в состоянии обслуживания"
              << " (scheduler_processing_time)" << std::endl;
    calculate_confidence_interval(
        scheduler_processing_time_history,
        mean_scheduler_processing_time,
        1);

    // FAIRNESS

    // Доверительный интервал для справделивости распределения RB между очередями
    std::cout << "\nОбщая справедливость распределения RB между очередями"
              << " (scheduler_fairness_for_queues)" << std::endl;
    calculate_confidence_interval(
        scheduler_fairness_for_queues_history,
        mean_scheduler_fairness_for_queues,
        0.001);

    // // задержка обработки пакетов по очередям
    // std::cout << "\nЗадержка обработки пакетов по очередям" << "\n";
    // for (size_t queue_id = 0; queue_id < mean_delay_by_queue_history.size(); ++queue_id)
    // {
    //     std::cout << "Oчередь №" << queue_id << "\n";
    //     double std_dev = calculate_standard_deviation_for_metric(
    //         mean_delay_by_queue_history[queue_id],
    //         mean_delays_by_queue[queue_id]);
    //     calculate_execution_count_for_metric(std_dev, 1);
    // }

    // for (size_t queue_id = 0; queue_id < mean_delay_by_queue_history.size(); ++queue_id){
    //     std::cout << "Oчередь №" << queue_id << "\n";
    //     std::cout << "Среднее: " << mean_delays_by_queue[queue_id] << "\n";
    //     for (auto& av : mean_delay_by_queue_history[queue_id]){
    //         std::cout << av << " ";
    //     }
    //     std::cout << "\n";
    // }
}

// Вывод в stdout срденего арифметического по статистике работы планировщика
void MeanStats::show()
{
    std::cout << "--------------\n\n";
    std::cout << "MEAN STATS\n";
    std::cout << "\nMean total time = "
              << mean_scheduler_total_time << " s\n" // Общее время работы
              << "Mean processing time = "
              << mean_scheduler_processing_time
              << " s (" // Общее время обслуживания пакетов и доля от общего времени
              << 100 * (mean_scheduler_processing_time / mean_scheduler_total_time)
              << "% of all)\n"
              << "Mean idle time = "
              << mean_scheduler_idle_time
              << " s (" // Общее время простоя и доля от общего времени
              << 100 * (mean_scheduler_idle_time / mean_scheduler_total_time)
              << "% of all)\n"
              << "Mean wait time = "
              << mean_scheduler_wait_time
              << " s (" // Общее время простоя и доля от общего времени
              << 100 * (mean_scheduler_wait_time / mean_scheduler_total_time)
              << "% of all)\n"
              << "Mean packet processing delay time = "
              << total_mean_delay_by_scheduler * 1000
              << " ms\n"; // Среднее время обслуживания пакета
}

std::string MeanStats::write_yaml()
{
    YAML::Emitter out;

    out << YAML::BeginMap;

    // mean_queue_processing_time
    out << YAML::Key << "mean_queue_processing_time"
        << YAML::Value << queue_mean_processing_time;

    // total_mean_queue_processing_time
    out << YAML::Key << "total_mean_queue_processing_time"
        << YAML::Value << total_mean_processing_time_by_scheduler;

    // mean_delays
    out << YAML::Key << "mean_delays"
        << YAML::Value << mean_delays_by_queue;

    // total_mean_delay
    out << YAML::Key << "total_mean_delay"
        << YAML::Value << total_mean_delay_by_scheduler;

    out << YAML::EndMap;

    std::string local_file_path = std::string(DATA_DIR) + std::string(FILE_NAME);
    std::string relative_file_path = "./" + local_file_path;
    std::ofstream fout(relative_file_path);

    fout << out.c_str();
    fout.close();

    std::string absolute_file_path =
        std::filesystem::current_path().c_str() + std::string("/") + local_file_path;
    return absolute_file_path;
}