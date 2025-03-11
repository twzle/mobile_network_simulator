#include "stats/mean_stats.hpp"

#include <chrono>
#include <thread>

#define DATA_DIR "data/"
#define FILE_NAME "data.yaml"

// Подсчет средних значений
void MeanStats::calculate()
{
    calculate_mean_values();
    collect_history();
}

void MeanStats::init_history()
{
    int iterations_count = stats_array.size();

    this->scheduler_total_time_history.reserve(iterations_count);
    this->scheduler_processing_time_history.reserve(iterations_count);
    this->scheduler_idle_time_history.reserve(iterations_count);
    this->scheduler_wait_time_history.reserve(iterations_count);

    this->scheduler_fairness_for_queues_history.reserve(iterations_count);
    this->scheduler_fairness_for_users_history.reserve(iterations_count);
    this->scheduler_throughput_history.reserve(iterations_count);

    this->scheduler_packet_processing_delay_history.reserve(iterations_count);

    init_queue_processing_delay_history();
}

void MeanStats::init_queue_processing_delay_history()
{
    int iterations_count = stats_array.size();
    for (size_t queue_id = 0;
         queue_id < stats_array[0].queue_average_packet_processing_delay.size();
         ++queue_id)
    {
        this->queue_packet_processing_delay_history[queue_id].reserve(iterations_count);
    }
}

// Сбор исторических данных по запускам для оптимизации доступа к данным
void MeanStats::collect_history()
{
    init_history();

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
        this->scheduler_fairness_for_users_history.push_back(
            stats.scheduler_average_fairness_for_users);
        this->scheduler_throughput_history.push_back(
            stats.scheduler_average_throughput);

        this->scheduler_packet_processing_delay_history.push_back(
            stats.scheduler_average_packet_processing_delay);
    }

    collect_queue_packet_processing_delay_history();
    collect_user_packet_processing_delay_history();
}

void MeanStats::collect_queue_packet_processing_delay_history()
{
    for (auto &stats : stats_array)
    {
        for (size_t queue_id = 0;
             queue_id < stats.queue_average_packet_processing_delay.size();
             ++queue_id)
        {
            this->queue_packet_processing_delay_history[queue_id]
                .push_back(stats.queue_average_packet_processing_delay[queue_id]);
        }
    }
}

void MeanStats::collect_user_packet_processing_delay_history()
{
    for (auto &stats : stats_array)
    {
        for (size_t user_id = 0;
             user_id < stats.user_average_packet_processing_delay.size();
             ++user_id)
        {
            this->user_packet_processing_delay_history[user_id]
                .push_back(stats.user_average_packet_processing_delay[user_id]);
        }
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
        common_scheduler_fairness_for_users += stats.scheduler_average_fairness_for_users;
        common_scheduler_throughput += stats.scheduler_average_throughput;

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
    mean_scheduler_fairness_for_users =
        common_scheduler_fairness_for_users / stats_array.size();
    mean_scheduler_throughput =
        common_scheduler_throughput / stats_array.size();

    mean_scheduler_packet_count =
        common_scheduler_packet_count / stats_array.size();

    calculate_mean_queue_packet_processing_delays();
    calculate_mean_user_packet_processing_delays();
    calculate_mean_scheduler_packet_processing_delay();
}

// Подсчет среднего арифметического задержек обработки пакетов
void MeanStats::calculate_mean_queue_packet_processing_delays()
{
    int queue_count = stats_array[0].queue_average_packet_processing_delay.size();

    for (int i = 0; i < queue_count; ++i)
    {
        mean_queue_packet_processing_delays[i] = 0;
    }

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            mean_queue_packet_processing_delays[i] +=
                stats.queue_average_packet_processing_delay[i];
        }

        mean_scheduler_packet_processing_delay +=
            stats.scheduler_average_packet_processing_delay;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        mean_queue_packet_processing_delays[i] /= stats_array.size();
    }

    mean_scheduler_packet_processing_delay /= stats_array.size();
}

// Подсчет среднего арифметического задержек обработки пакетов по пользователям
void MeanStats::calculate_mean_user_packet_processing_delays()
{
    int user_count = stats_array[0].user_average_packet_processing_delay.size();

    for (int i = 0; i < user_count; ++i)
    {
        mean_user_packet_processing_delays[i] = 0;
    }

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < user_count; ++i)
        {
            mean_user_packet_processing_delays[i] +=
                stats.user_average_packet_processing_delay[i];
        }
    }

    for (int i = 0; i < user_count; ++i)
    {
        mean_user_packet_processing_delays[i] /= stats_array.size();
    }
}

// Подсчет среднего арифметического задержек обработки пакетов для планировщика
void MeanStats::calculate_mean_scheduler_packet_processing_delay()
{
    for (auto &stats : stats_array)
    {
        mean_scheduler_packet_processing_delay +=
            stats.scheduler_average_packet_processing_delay;
    }

    mean_scheduler_packet_processing_delay /= stats_array.size();
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
        double value_deviation = 0;
        if (values.size() == 1)
        {
            value_deviation = mean;
        }
        else if (values.size() > 1)
        {
            value_deviation = value - mean;
        }

        sum_of_squared_deviations += value_deviation * value_deviation;
    }

    // sum(Xi - X)^2 / (n - 1)
    // Квадрат среднеквадратического отклонения
    double square_of_standard_deviation = 0;
    if (values.size() == 1)
    {
        square_of_standard_deviation =
            sum_of_squared_deviations / (stats_array.size());
    }
    else if (values.size() > 1)
    {
        square_of_standard_deviation =
            sum_of_squared_deviations / (stats_array.size() - 1);
    }

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

    // Доверительный интервал для справделивости распределения RB между очередями
    std::cout << "\nОбщая справедливость распределения RB между пользователями"
              << " (scheduler_fairness_for_users)" << std::endl;
    calculate_confidence_interval(
        scheduler_fairness_for_users_history,
        mean_scheduler_fairness_for_users,
        0.001);

    // Доверительный интервал для справделивости распределения RB между очередями
    std::cout << "\nОбщая пропускная способность"
              << " (scheduler_throughput)" << std::endl;
    calculate_confidence_interval(
        scheduler_throughput_history,
        mean_scheduler_throughput,
        0.001);

    // Доверительный интервал для средней задержки обслуживания пакетов
    std::cout << "\nОбщая средняя задержка обслуживания пакетов"
              << " (scheduler_packet_processing_delay)" << std::endl;
    calculate_confidence_interval(
        scheduler_packet_processing_delay_history,
        mean_scheduler_packet_processing_delay,
        0.00001);

    evaluate_confidence_queue_packet_processing_delay_intervals();
    evaluate_confidence_user_packet_processing_delay_intervals();
}

void MeanStats::evaluate_confidence_queue_packet_processing_delay_intervals()
{
    for (size_t queue_id = 0;
         queue_id < queue_packet_processing_delay_history.size();
         ++queue_id)
    {
        // Задержка обработки пакетов по очередям
        std::cout << "\nОбщая средняя задержка обслуживания пакетов в очереди №"
                  << queue_id + 1
                  << " (queue_packet_processing_delay)" << std::endl;
        calculate_confidence_interval(
            queue_packet_processing_delay_history[queue_id],
            mean_queue_packet_processing_delays[queue_id],
            0.00001);
    }
}

void MeanStats::evaluate_confidence_user_packet_processing_delay_intervals()
{
    for (size_t user_id = 0;
         user_id < user_packet_processing_delay_history.size();
         ++user_id)
    {
        // Задержка обработки пакетов по пользователям
        std::cout << "\nОбщая средняя задержка обслуживания пакетов пользователя №"
                  << user_id + 1
                  << " (user_packet_processing_delay)" << std::endl;
        calculate_confidence_interval(
            user_packet_processing_delay_history[user_id],
            mean_user_packet_processing_delays[user_id],
            0.00001);
    }
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
              << "Mean fairness of RB allocation for queues = "
              << mean_scheduler_fairness_for_queues
              << "\n" // Средняя справедливость распределения RB по очередям
              << "Mean fairness of RB allocation for users = "
              << mean_scheduler_fairness_for_users
              << "\n" // Средняя справедливость распределения RB по пользователям
              << "Mean scheduler throughput = "
              << mean_scheduler_throughput
              << " Kbytes/ms, " // Средняя пропускная способность (Кбайт/мс)
              << mean_scheduler_throughput * 1024 * 1000
              << " bytes/s\n" // Средняя пропускная способность (байт/с)
              << "Mean scheduler packet processing delay time = "
              << mean_scheduler_packet_processing_delay * 1000
              << " ms\n"; // Среднее время обслуживания пакета

    for (auto& queue : queue_packet_processing_delay_history)
    {
        std::cout << "Mean queue packet processing delay time "
                  << "(Queue #" << queue.first<< ") = "
                  << mean_queue_packet_processing_delays[queue.first] * 1000 << " ms\n";
    }

    for (auto& user : user_packet_processing_delay_history)
   {
       std::cout << "Mean user packet processing delay time "
                 << "(User #" << user.first << ") = "
                 << mean_user_packet_processing_delays[user.first] * 1000 << " ms\n";
   }
}

std::string MeanStats::write_yaml()
{
    YAML::Emitter out;

    out << YAML::BeginMap;

    // Average delays

    // Scheduler
    out << YAML::Key << "scheduler_packet_processing_delay"
        << YAML::Value << mean_scheduler_packet_processing_delay;

    // Queues
    out << YAML::Key << "queue_packet_processing_delays"
        << YAML::Value << mean_queue_packet_processing_delays;

    // Users
    out << YAML::Key << "user_packet_processing_delays"
        << YAML::Value << mean_user_packet_processing_delays;

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