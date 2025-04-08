#pragma once

#include <vector>
#include <map>

class TBS {
    public:
        static void initialize();
        static int find_min_rb_for_packet(int itbs, int packet_bytes);

    private:
        static std::vector<std::vector<std::pair<int, int> > > tbs_table;
};