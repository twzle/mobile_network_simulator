#include "packet_queue_scheduler.hpp"
// #include "matplotlibcpp.h"


int main(){
    // Инициализация пакетов
    Packet p1(5);
    Packet p2(2);
    Packet p3(8);
    Packet p4(19);
    
    // Инициализация очереди №1
    PacketQueue pq1(20, 3);
    pq1.add_packet(p1);
    pq1.add_packet(p2);
    pq1.add_packet(p3);
    pq1.add_packet(p4);

    // Инициализация очереди №2
    PacketQueue pq2(1, 2);
    pq2.add_packet(p3);
    pq2.add_packet(p4);
    pq2.add_packet(p2);
    pq2.add_packet(p1);

    // Инициализация планировщика
    PacketQueueScheduler pqs(pq1);
    pqs.schedule(pq2);
    pqs.schedule(pq1);
    pqs.schedule(pq2);
    pqs.run();

    return 0;
}