#pragma once

class Settings {
    public:
        Settings() = default;
        Settings(
            int launches, 
            int packet_count, 
            int packet_size, 
            int queue_count, 
            int queue_quant,
            int queue_limit);

        int get_launches();
        int get_packet_count();
        int get_packet_size();
        int get_queue_count();
        int get_queue_quant();
        int get_queue_limit();

    
    private:
        int launches; // Количество повторов
        int packet_count; // Количество пакетов для каждого повтора
        int packet_size; // Размеры пакетов для каждого повтора
        int queue_count; // Количество очередей для каждого повтора
        int queue_quant; // Квант времени очередей для каждого повтора
        int queue_limit; // Размеры очередей для каждого повтора
};