#include "config/tbs.hpp"

#include <algorithm> // для std::lower_bound

// Функция для поиска минимального RB по ITBS и размеру пакета (в байтах)
int TBS::find_min_rb_for_packet(int itbs, int packet_bytes) {
    // 1. Проверка валидности ITBS
    if (itbs < 0 || itbs >= (int) tbs_table.size()) {
        return -1;
    }

    // 2. Получаем подтаблицу для заданного ITBS
    std::vector<std::pair<int, int> > tbs_row = tbs_table[itbs];

    // 3. Конвертируем байты в биты (TBS в таблице — всегда в битах)
    int packet_bits = packet_bytes * 8;

    // 4. Ищем первый TBS >= packet_bits (бинарный поиск)
    auto it = std::lower_bound(
        tbs_row.begin(), 
        tbs_row.end(), 
        packet_bits,
        [](const std::pair<int, int>& entry, int value) {
            return entry.first < value;
        }
    );

    return (it != tbs_row.end()) ? it->second : -1;
}