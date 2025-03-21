#pragma once

// Ширина RB в МГц
#define RB_BANDWIDTH 0.180
// Максимальное расстояние между базовой станцией и пользователем согласно СOST231 (метры)
#define BS_TO_UE_DISTANCE_MAX 20000
// Максимальное расстояние между базовой станцией и пользователем согласно СOST231 (метры)
#define BS_TO_UE_DISTANCE_MIN 1000

#define THROUGHPUT_MIN 0.001

const int CARRIER_FREQUENCY = 2000; // Несущая частота в МГЦ

const double DEFICIT_MAX = 1000000; // Максимальный дефицит

const double epsilon = 1e-9;