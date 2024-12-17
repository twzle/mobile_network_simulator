#include "core/executor.hpp"
#include "core/time_generator.hpp"
#include "scheduling/default_drr_scheduler.hpp"
#include "scheduling/fixed_drr_scheduler.hpp"
#include "scheduling/circular_drr_scheduler.hpp"


Executor::Executor(Settings settings){
    this->settings = settings;
    std::exponential_distribution<> time_distribution(settings.get_time_lambda());
    TimeGenerator::set_distribution(
        std::exponential_distribution<>(settings.get_time_lambda())
    );
}

void Executor::run(){
    for (int i = 0; i < settings.get_launches(); ++i){
        TimeGenerator::reset_time();

        std::cout << "Running scheduler #" << i + 1 << "...\n";
        this->execute();
    }
}

void Executor::execute(){
    auto scheduler = settings.get_scheduler_instance();
            
    // Наполнение очередей пакетами
    for (int i = 0; i < settings.get_queue_count(); ++i){
        PacketQueue queue(
            settings.get_queue_quant(), 
            settings.get_queue_limit()
        );

        for (int j = 0; j < settings.get_packet_count(); ++j){
            Packet packet(settings.get_packet_size());
            queue.add_packet(packet);
        }

        // Планирование обслуживания очереди
        scheduler->schedule(std::move(queue));
        scheduler->set_resource_block_per_tti_limit(
            settings.get_resource_block_per_tti_limit()
        );
    }

    scheduler->run();

    ExecutionStats& stats = scheduler->get_stats();
    stats.evaluate();
    stats.release_memory_resources();
    stats.print();

    // Запись статистики текущего запуска в массив
    this->average_stats.stats_array.push_back(std::move(stats));
}

AverageStats& Executor::get_stats(){
    return this->average_stats;
}