#include "packet_queue_scheduler.hpp"
#include "time_generator.hpp"
#include <random>


int main(){
    TimeGenerator::initialize();

    // Параметры очередей
    int packet_count = 5000;
    int packet_size = 1;
    int queue_count = 4;
    int quant = 1;
    int limit = 10000;

    std::vector<PacketQueue> queues;
    PacketQueueScheduler pqs;
    
    // Наполнение очередей пакетами
    for (int i = 0; i < queue_count; ++i){
        PacketQueue queue(quant, limit);

        for (int i = 0; i < packet_count; ++i){
            Packet packet(packet_size);
            queue.add_packet(packet);
        }

        // Планирование обслуживания очереди
        pqs.schedule(queue);
    }

    // Запуск планировщика
    pqs.run();

    return 0;
}