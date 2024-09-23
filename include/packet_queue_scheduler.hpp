#include "packet_queue.hpp"

class Stats {
    public:
        Stats() : 
            total_time(0), total_skip_time(0),
            total_processing_time(0), packet_count(0){}
        
        void summarize(){

            auto total_idle_time = total_time.count() - total_processing_time.count();

            std::cout << "Total running time = " 
                << total_time.count() << " ms\n"
                << "Total processing time = " 
                << total_processing_time.count() << " ms ("
                <<  100 * ((float) total_processing_time.count() / (float) total_time.count()) << "% of all)\n"
                << "Total idle time = " 
                << total_idle_time << " ms ("
                <<  100 * ((float) total_idle_time / (float) total_time.count()) << "% of all)\n"
                << "Average packet processing time = " 
                << total_processing_time.count() / packet_count << " ms\n";
        }

        ms total_time;
        ms total_skip_time;
        ms total_processing_time;
        int packet_count;
};

class PacketQueueScheduler {
    public:
        PacketQueueScheduler(PacketQueue& packet_queue);

        void run();
        void schedule(const PacketQueue& packet_queue);

    private:
        int total_packets = 0;
        std::vector<PacketQueue> scheduled_queues;
        Stats stats;
};