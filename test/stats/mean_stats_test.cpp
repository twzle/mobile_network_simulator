#include <gtest/gtest.h>
#include "stats/mean_stats.hpp"
#include "stats/iteration_stats.hpp"

class MeanStatsTest : public ::testing::Test
{
protected:
    bool initialized;
    void lazy_initialize()
    {
        if (!initialized)
        {
            // Создаем тестовые данные IterationStats
            IterationStats stats1, stats2;

            // Заполняем первую статистику
            stats1.scheduler_total_time = 1.0;
            stats1.scheduler_processing_time = 0.6;
            stats1.scheduler_idle_time = 0.3;
            stats1.scheduler_wait_time = 0.1;
            stats1.scheduler_average_fairness_for_queues = 0.8;
            stats1.scheduler_average_fairness_for_users = 0.7;
            stats1.scheduler_average_throughput = 10.0;
            stats1.scheduler_average_unused_resources = 0.2;
            stats1.packet_count = 100;
            stats1.scheduler_average_packet_processing_delay = 0.01;
            stats1.queue_average_packet_processing_delay[0] = 0.02;
            stats1.user_average_packet_processing_delay[0] = 0.03;

            // Заполняем вторую статистику
            stats2.scheduler_total_time = 2.0;
            stats2.scheduler_processing_time = 1.2;
            stats2.scheduler_idle_time = 0.6;
            stats2.scheduler_wait_time = 0.2;
            stats2.scheduler_average_fairness_for_queues = 0.9;
            stats2.scheduler_average_fairness_for_users = 0.8;
            stats2.scheduler_average_throughput = 20.0;
            stats2.scheduler_average_unused_resources = 0.3;
            stats2.packet_count = 200;
            stats2.scheduler_average_packet_processing_delay = 0.02;
            stats2.queue_average_packet_processing_delay[0] = 0.04;
            stats2.user_average_packet_processing_delay[0] = 0.06;

            // Добавляем в MeanStats
            meanStats.stats_array.push_back(std::move(stats1));
            meanStats.stats_array.push_back(std::move(stats2));

            initialized = true;
        }
    }

    void SetUp() override
    {
        lazy_initialize();
    }

    MeanStats meanStats;
};

TEST_F(MeanStatsTest, CalculateMeanValues)
{
    meanStats.calculate_mean_values();

    // Проверяем средние значения
    EXPECT_DOUBLE_EQ(1.5, meanStats.mean_scheduler_total_time);      // (1.0 + 2.0)/2
    EXPECT_DOUBLE_EQ(0.9, meanStats.mean_scheduler_processing_time); // (0.6 + 1.2)/2
    EXPECT_DOUBLE_EQ(0.45, meanStats.mean_scheduler_idle_time);      // (0.3 + 0.6)/2
    EXPECT_DOUBLE_EQ(0.15, meanStats.mean_scheduler_wait_time);      // (0.1 + 0.2)/2

    EXPECT_DOUBLE_EQ(0.85, meanStats.mean_scheduler_fairness_for_queues); // (0.8 + 0.9)/2
    EXPECT_DOUBLE_EQ(0.75, meanStats.mean_scheduler_fairness_for_users);  // (0.7 + 0.8)/2
    EXPECT_DOUBLE_EQ(15.0, meanStats.mean_scheduler_throughput);          // (10.0 + 20.0)/2
    EXPECT_DOUBLE_EQ(0.25, meanStats.mean_scheduler_unused_resources);    // (0.2 + 0.3)/2

    EXPECT_EQ(150, meanStats.mean_scheduler_packet_count); // (100 + 200)/2

    EXPECT_DOUBLE_EQ(0.015, meanStats.mean_scheduler_packet_processing_delay); // (0.01 + 0.02)/2
    EXPECT_DOUBLE_EQ(0.03, meanStats.mean_queue_packet_processing_delays[0]);  // (0.02 + 0.04)/2
    EXPECT_DOUBLE_EQ(0.045, meanStats.mean_user_packet_processing_delays[0]);  // (0.03 + 0.06)/2
}


// TEST_F(MeanStatsTest, CalculateStandardDeviation)
// {
//     std::vector<double> values = {10.0, 20.0};
//     double mean = 15.0;

//     double stdDev = meanStats.calculate_standard_deviation_for_metric(values, mean);

//     // Проверяем расчет стандартного отклонения
//     // sqrt(((10-15)^2 + (20-15)^2) / (2-1)) = sqrt(50) ≈ 7.071
//     EXPECT_NEAR(7.071, stdDev, 0.001);
// }

TEST_F(MeanStatsTest, CalculateExecutionCount)
{
    // Тестируем через calculate_confidence_interval
    std::vector<double> values = {10.0, 20.0};
    double mean = 15.0;
    double accuracy = 1.0;

    // Проверяем, что не падает
    EXPECT_NO_THROW(meanStats.calculate_confidence_interval(values, mean, accuracy));
}

TEST_F(MeanStatsTest, CalculateMaxThroughput)
{
    StandardManager::initialize();
    TBS::initialize();
    double bandwidth = 20.0; // MHz
    meanStats.calculate_max_scheduler_throughput(bandwidth);

    // Проверяем, что максимальная пропускная способность рассчитана
    EXPECT_GT(meanStats.max_scheduler_throughput, 0);
}

TEST_F(MeanStatsTest, ShowDelays)
{
    meanStats.calculate_mean_values();

    // Проверяем, что методы show не падают
    EXPECT_NO_THROW(meanStats.show_queue_delays());
    EXPECT_NO_THROW(meanStats.show_user_delays());
}

TEST_F(MeanStatsTest, WriteYaml)
{
    meanStats.calculate_mean_values();

    // Проверяем запись в YAML
    std::string filePath = meanStats.write_yaml();
    EXPECT_FALSE(filePath.empty());

    // Проверяем, что файл создан
    EXPECT_TRUE(std::filesystem::exists(filePath));
}

TEST_F(MeanStatsTest, EmptyStatsArray)
{
    MeanStats emptyStats;

    // Проверяем обработку пустого массива статистик
    EXPECT_NO_THROW(emptyStats.calculate_mean_values());
    EXPECT_NO_THROW(emptyStats.collect_history());
}