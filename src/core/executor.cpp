#include "core/executor.hpp"
#include "core/time_generator.hpp"
#include "core/user_generator.hpp"
#include "scheduling/default_drr_scheduler.hpp"
#include "scheduling/fixed_drr_scheduler.hpp"
#include "scheduling/cyclic_drr_scheduler.hpp"

Executor::Executor(Settings settings)
{
    this->settings = settings;

    TimeGenerator::set_distribution(
        std::exponential_distribution<>(settings.get_time_lambda()));
    UserGenerator::set_distribution(
        std::uniform_int_distribution<>(1, settings.get_user_count()));
}

void Executor::run()
{
    for (int i = 0; i < settings.get_launches(); ++i)
    {
        TimeGenerator::reset_time();
        User::reset_last_id();

        std::cout << "Running scheduler #" << i + 1 << "...\n";
        this->execute();
    }
}

void Executor::execute()
{
    auto scheduler = settings.get_scheduler_instance();
    scheduler->configure_base_station(settings.get_bs_config());
    scheduler->connect_users(settings.get_user_configs());

    // Наполнение очередей пакетами
    for (int queue_id = 0; queue_id < settings.get_queue_count(); ++queue_id)
    {
        TimeGenerator::reset_time();
        
        PacketQueue queue(
            settings.get_queue_quant(),
            settings.get_queue_limit());

        for (int j = 0; j < settings.get_packet_count(); ++j)
        {
            User *user = scheduler->get_user_ptr(UserGenerator::generate_user_id());
            if (user != nullptr)
            {
                Packet packet(queue_id, settings.get_packet_size(), user);
                queue.add_packet(packet);
            }
        }

        // Планирование обслуживания очереди
        scheduler->schedule(std::move(queue));
        scheduler->set_resource_block_per_tti_limit(
            settings.get_resource_block_per_tti_limit());
    }

    scheduler->run();

    IterationStats &stats = scheduler->get_stats();
    stats.evaluate();
    stats.release_memory_resources();
    stats.print();

    // Запись статистики текущего запуска в массив
    this->mean_stats.stats_array.push_back(std::move(stats));
}

MeanStats &Executor::get_stats()
{
    return this->mean_stats;
}