#include "executor.hpp"
#include "packet_queue_scheduler.hpp"
#include "time_generator.hpp"
#include <thread>   // Для sleep_for и chrono
#include <chrono>   // Для единиц времени


Executor::Executor(Settings settings){
    this->settings = settings;
    std::exponential_distribution<> time_distribution(settings.get_time_lambda());
    TimeGenerator::set_distribution(std::exponential_distribution<>(settings.get_time_lambda()));
}

void Executor::run(){
    for (int i = 0; i < settings.get_launches(); ++i){
        TimeGenerator::reset_time();

        std::cout << "Running scheduler #" << i + 1 << "...\n";
        this->execute();
    }
}

void Executor::execute(){
    PacketQueueScheduler pqs;
            
    // Наполнение очередей пакетами
    for (int i = 0; i < settings.get_queue_count(); ++i){
        PacketQueue queue(
            settings.get_queue_quant(), 
            settings.get_queue_limit()
        );

        for (int i = 0; i < settings.get_packet_count(); ++i){
            Packet packet(settings.get_packet_size());
            queue.add_packet(packet);
        }

        // Планирование обслуживания очереди
        pqs.schedule(std::move(queue));
    }

    pqs.run();

    ExecutionStats& stats = pqs.get_stats();
    stats.evaluate();
    stats.release_memory_resources();
    stats.print();

    // Запись статистики текущего запуска в массив
    this->average_stats.stats_array.push_back(std::move(stats));
}

AverageStats& Executor::get_stats(){
    return this->average_stats;
}