#pragma once

#include <vector>
#include <map>

class TBS {
    public:
        static void initialize();
        static void clear();
        static int find_min_rb_for_packet(int itbs, int packet_bytes);
        static int get_size_for_rb(int itbs, int rb);
        static std::vector<std::vector<std::pair<int, int> > > get_table();
        static int get_table_size();

    private:
        static std::vector<std::vector<std::pair<int, int> > > tbs_table;
};