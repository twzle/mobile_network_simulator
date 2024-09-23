#include "packet_queue_scheduler.hpp"


int main(){
    Packet p1(5);
    Packet p2(2);
    Packet p3(8);
    Packet p4(19);
    
    PacketQueue pq1(20);
    pq1.add_packet(p1);
    pq1.add_packet(p2);
    pq1.add_packet(p3);


    PacketQueue pq2(1);
    pq2.add_packet(p3);
    pq2.add_packet(p4);

    PacketQueueScheduler pqs(pq1);
    pqs.schedule(pq2);
    pqs.run();

    return 0;
}