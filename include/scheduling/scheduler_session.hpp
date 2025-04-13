#pragma once

class SchedulerSession
{
public:
    SchedulerSession() = default;
    ~SchedulerSession() = default;

    void set_scheduling_start_time(double time);
    void set_scheduling_end_time(double time);
    double get_scheduling_start_time() const;
    double get_scheduling_end_time() const;

    void increment_processed_packet_count(int increment);
    int get_processed_packet_count() const;

private:
    double scheduling_start = 0; // Начало работы планировщика
    double scheduling_end = 0;   // Конец работы планировщика
    int processed_packets = 0;   // Общее число обслуженных пакетов
};