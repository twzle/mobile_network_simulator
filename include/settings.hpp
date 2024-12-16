#pragma once

#include <stdexcept>

class Settings {
    public:
        Settings() = default;
        Settings(
            int launches,
            double bandwidth, 
            int packet_count, 
            int packet_size, 
            int queue_count, 
            int queue_quant,
            int queue_limit,
            double time_lambda);

        void validate();

        int get_launches();
        double get_bandwidth();
        int get_packet_count();
        int get_packet_size();
        int get_queue_count();
        int get_queue_quant();
        int get_queue_limit();
        double get_time_lambda();
        int get_resource_block_per_tti_limit();

    private:
        int launches; // Количество повторов
        double bandwidth; // Полоса пропускания в МГц
        int packet_count; // Количество пакетов для каждого повтора
        int packet_size; // Размеры пакетов для каждого повтора
        int queue_count; // Количество очередей для каждого повтора
        int queue_quant; // Квант времени очередей для каждого повтора
        int queue_limit; // Размеры очередей для каждого повтора
        double time_lambda; // Частота прихода пакетов в единицу времени
};