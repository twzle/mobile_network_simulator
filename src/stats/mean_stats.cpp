#include "stats/mean_stats.hpp"

#define DATA_DIR "data/"
#define FILE_NAME "data.yaml"

// Подсчет средних значений
void MeanStats::calculate()
{
    calculate_mean_values();
    calculate_mean_delays();
    calculate_mean_queue_processing_time();

    collect_history();

    // Общая задержка обработки пакетов
    // std::cout << "\nОбщая задержка обработки пакетов" << "\n";
    // double std_dev = calculate_standard_deviation_for_metric(
    //     mean_delay_by_scheduler_history,
    //     total_mean_delay_by_scheduler);
    // calculate_execution_count_for_metric(std_dev, 1);

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

// Сбор исторических данных по запускам для оптимизации доступа к данным
void MeanStats::collect_history()
{
    for (auto &stats : stats_array)
    {
        this->mean_delay_by_scheduler_history
            .push_back(stats.average_delay_by_scheduler);
        this->mean_processing_time_by_scheduler_history
            .push_back(stats.scheduler_average_total_time);

        for (size_t queue_id = 0; queue_id < stats.average_delay_by_queue.size(); ++queue_id)
        {
            this->mean_delay_by_queue_history[queue_id]
                .push_back(stats.average_delay_by_queue[queue_id]);
        }

        for (size_t queue_id = 0;
             queue_id < stats.queue_total_time.size();
             ++queue_id)
        {
            this->processing_time_by_queue_history[queue_id]
                .push_back(stats.queue_total_time[queue_id]);
        }
    }

    // std::cout << "Delay history size: " << mean_delay_by_scheduler_history.size() << "\n";
    // std::cout << "Delay history queues: " << mean_delay_by_queue_history.size() << "\n";
    // std::cout << "Delay history queue size: " << mean_delay_by_queue_history[0].size() << "\n";
    // std::cout << "Proc time history size: " << mean_processing_time_by_scheduler_history.size() << "\n";
    // std::cout << "Proc time history queues: " << processing_time_by_queue_history.size() << "\n";
    // std::cout << "Proc time history queue size: " << processing_time_by_queue_history[0].size() << "\n";
}

// Подсчет среднего арифметического базовых параметров работы планировщика
void MeanStats::calculate_mean_values()
{
    for (auto &stats : stats_array)
    {
        common_total_time += stats.scheduler_total_time;
        common_total_idle_time += stats.scheduler_idle_time;
        common_total_processing_time += stats.scheduler_processing_time;
        common_total_wait_time += stats.scheduler_wait_time;
        common_total_packet_count += stats.packet_count;
    }

    mean_total_time =
        common_total_time / stats_array.size();
    mean_total_idle_time =
        common_total_idle_time / stats_array.size();
    mean_total_processing_time =
        common_total_processing_time / stats_array.size();
    mean_total_wait_time =
        common_total_wait_time / stats_array.size();
    mean_total_packet_count =
        common_total_packet_count / stats_array.size();
}

// Подсчет среднего арифметического задержек обработки пакетов
void MeanStats::calculate_mean_delays()
{
    int queue_count = stats_array[0].average_delay_by_queue.size();
    mean_delays_by_queue.resize(queue_count);

    for (auto &stats : stats_array)
    {
        for (int i = 0; i < queue_count; ++i)
        {
            mean_delays_by_queue[i] += stats.average_delay_by_queue[i];
        }
        total_mean_delay_by_scheduler += stats.average_delay_by_scheduler;
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
            stats.scheduler_average_total_time;
    }

    for (int i = 0; i < queue_count; ++i)
    {
        queue_mean_processing_time[i] /= stats_array.size();
    }

    total_mean_processing_time_by_scheduler = total_mean_processing_time / stats_array.size();
}

// Подсчет минимального числа запусков для уровня достоверности
void MeanStats::calculate_execution_count_for_metric(
    const double &standard_deviation, const double &accuracy)
{
    double credability_out_of_95 = 1.96;  // z
    double credability_out_of_99 = 2.576; // z

    // (z * S / E)^2
    double product_for_95 = (credability_out_of_95 * standard_deviation / accuracy);

    double product_for_99 = (credability_out_of_99 * standard_deviation / accuracy);

    double launches_for_95 = std::pow(product_for_95, 2);
    double launches_for_99 = std::pow(product_for_99, 2);

    std::cout << "E (допустимая погрешность) = " << accuracy << "\n";
    std::cout << "Количество запусков для достоверности 95% = " << launches_for_95 << "\n";
    std::cout << "Количество запусков для достоверности 99% = " << launches_for_99 << "\n";
}

// Подсчет стандартного отклонения величины
double MeanStats::calculate_standard_deviation_for_metric(
    const std::vector<double> &values, const double &mean)
{
    // sum(Xi - X)^2
    double total_deviation = 0;
    for (auto &value : values)
    {
        double difference = value - mean;
        total_deviation += difference * difference;
    }

    // sum(Xi - X)^2 / (n - 1)
    double square_of_deviation = total_deviation / (stats_array.size() - 1);

    // sqrt(sum(Xi - X)^2 / (n - 1))
    double standard_devaition = std::sqrt(square_of_deviation);
    std::cout << "Стандартное отклонение величины = " << standard_devaition << "\n";

    return standard_devaition;
}

// Вывод в stdout срденего арифметического по статистике работы планировщика
void MeanStats::show()
{
    std::cout << "--------------\n\n";
    std::cout << "mean stats\n";
    std::cout << "\nRunning time = "
              << mean_total_time << " ms\n" // Общее время работы
              << "Processing time = "
              << mean_total_processing_time
              << " ms (" // Общее время обслуживания пакетов и доля от общего времени
              << 100 * (mean_total_processing_time / mean_total_time)
              << "% of all)\n"
              << "Common total idle time = "
              << mean_total_idle_time
              << " ms (" // Общее время простоя и доля от общего времени
              << 100 * (mean_total_idle_time / mean_total_time)
              << "% of all)\n"
              << "mean packet processing time = "
              << mean_total_processing_time / mean_total_packet_count
              << " ms\n" // Среднее время обслуживания пакета
              << "mean packet delay time = "
              << total_mean_delay_by_scheduler
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