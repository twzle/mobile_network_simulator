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
    UserGenerator::set_user_id_distribution(
        std::uniform_int_distribution<>(1, settings.get_user_count()));
    UserGenerator::set_user_move_direction_distribution(
        std::uniform_int_distribution<>(1, 4));
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

std::unique_ptr<BaseScheduler> Executor::initialize_scheduler(){
    auto scheduler = settings.get_scheduler_instance();

    scheduler->set_base_station(settings.get_bs_config());
    scheduler->set_users(settings.get_user_configs());
    scheduler->set_resource_block_per_tti_limit(
        settings.get_resource_block_per_tti_limit());
    scheduler->set_tti_duration(settings.get_tti_value());
    scheduler->set_channel_sync_interval(settings.get_channel_sync_interval_value());
    scheduler->set_base_cqi(settings.get_base_cqi());
    scheduler->set_channel(
        Channel(
            settings.get_carrier_frequency(), 
            settings.get_bs_transmission_power(),
            settings.get_area_type()
        )
    );

    return scheduler;
}

void Executor::execute()
{
    auto scheduler = initialize_scheduler();

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
                queue.schedule_packet(packet);
            }
        }

        // Планирование обслуживания очереди
        scheduler->schedule(std::move(queue));
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