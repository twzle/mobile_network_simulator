#include <map>
#include <vector>
#include "types.hpp"

class PacketStats {
    public:
        PacketStats(ms scheduled_at, ms processing_delay);

        ms scheduled_at;
        ms processing_delay;
};

class Stats {
    public:
        Stats();
        
        void summarize();
        void draw_delay_plot();
        void draw_scheduling_plot();

        ms total_time; // Общее время работы планировщика в мс
        ms total_skip_time; // Общее время простоя планировщика в мс
        ms total_processing_time; // Общее время обслуживания пакетов в мс
        int packet_count; // Общее число обслуженных пакетов
        int retried_packet_count; // Число обслуженных пакетов не с первого раза
        int lost_packet_count; // Число потерянных пакетов

        std::map<int, std::vector<PacketStats>> queue_stats;
};