#include "packet_queue_scheduler.hpp"

PacketQueueScheduler::PacketQueueScheduler(PacketQueue &packet_queue)
{
    this->schedule(packet_queue);
}

void PacketQueueScheduler::run()
{
    auto run_start = std::chrono::high_resolution_clock::now();
    int served_packets = 0;

    while (served_packets < this->total_packets)
    {
        int queue_id = 0;

        for (auto &queue : scheduled_queues)
        {
            std::cout << "Queue #" << queue_id + 1 << "\n";
            queue.set_deficit(queue.get_deficit() + queue.get_quant());

            while (queue.size() > 0)
            {
                ms packet_size = queue.front().get_size();
                ms deficit = queue.get_deficit();

                if (packet_size <= deficit)
                {
                    auto packet_processing_start =
                        std::chrono::high_resolution_clock::now();
                    ms sleep_duration(packet_size);
                    std::this_thread::sleep_for(sleep_duration);
                    auto packet_processing_end = std::chrono::high_resolution_clock::now();

                    queue.pop();
                    ++served_packets;
                    queue.set_deficit(deficit - packet_size);

                    auto packet_processing_duration =
                        std::chrono::duration_cast<ms>(packet_processing_end - packet_processing_start);

                    stats.total_processing_time += packet_processing_duration;

                    queue.set_deficit(deficit - packet_processing_duration);

                    std::cout << "Packet of size " << packet_size.count()
                              << " served in " << packet_processing_duration.count() << " ms."
                              << " Remaining deficit: " << queue.get_deficit().count()
                              << "\n";
                }
                else
                {
                    break;
                }
            }

            queue_id++;
        }
    }

    auto run_end = std::chrono::high_resolution_clock::now();
    auto run_duration =
        std::chrono::duration_cast<ms>(run_end - run_start);

    stats.total_time = run_duration;
    stats.packet_count = total_packets;
    stats.summarize();
}

void PacketQueueScheduler::schedule(const PacketQueue &packet_queue)
{
    scheduled_queues.push_back(packet_queue);
    total_packets += packet_queue.size();
}