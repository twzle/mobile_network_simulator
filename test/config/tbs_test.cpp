#include "config/tbs.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <stdio.h>

class TBSTest : public ::testing::Test
{
protected:
    static bool initialized;
    static void lazy_initialize()
    {
        if (!initialized)
        {
            TBS::initialize();
            initialized = true;
        }
    }

    void SetUp() override
    {
        lazy_initialize();
    }
};

bool TBSTest::initialized = false;

// Тест find_min_rb_for_packet с валидными данными
TEST_F(TBSTest, FindMinRbValidCases)
{
    // Точные соответствия
    EXPECT_EQ(TBS::find_min_rb_for_packet(0, 368 / 8), 14); // 368 бит = 46 байт
    EXPECT_EQ(TBS::find_min_rb_for_packet(1, 1000 / 8), 28);

    // Промежуточные значения (выбираем больший RB)
    EXPECT_EQ(TBS::find_min_rb_for_packet(0, 400 / 8), 16);  // 400 > 368
    EXPECT_EQ(TBS::find_min_rb_for_packet(2, 1100 / 8), 25); // 1100 > 1064

    // Граничные случаи
    EXPECT_EQ(TBS::find_min_rb_for_packet(1, 2088 / 8), 57); // Последний элемент
    EXPECT_EQ(TBS::find_min_rb_for_packet(2, 2000 / 8), 45); // Между 1624 и 2216
}

// Тест find_min_rb_for_packet с невалидными данными
TEST_F(TBSTest, FindMinRbInvalidCases)
{
    // Невалидный ITBS
    EXPECT_EQ(TBS::find_min_rb_for_packet(-1, 100), -1);
    EXPECT_EQ(TBS::find_min_rb_for_packet(100, 100), -1);

    // Слишком большой пакет
    EXPECT_EQ(TBS::find_min_rb_for_packet(0, 100000), -1);   // Нет подходящего RB
    EXPECT_EQ(TBS::find_min_rb_for_packet(2, -100 / 8), -1); // Больше максимального
}

// Тест get_size_for_rb с валидными данными
TEST_F(TBSTest, GetSizeForRbValidCases)
{
    EXPECT_EQ(TBS::get_size_for_rb(0, 1), 2); // Первый элемент
    EXPECT_EQ(TBS::get_size_for_rb(1, 2), 7);
    EXPECT_EQ(TBS::get_size_for_rb(2, 4), 22); // Последний элемент

    // Проверка округления (если бы были некратные 8 значения)
    EXPECT_EQ(TBS::get_size_for_rb(1, 1), 3);
}

// Тест get_size_for_rb с невалидными данными
TEST_F(TBSTest, GetSizeForRbInvalidCases)
{
    // Невалидный ITBS
    EXPECT_EQ(TBS::get_size_for_rb(-1, 1), -1);
    EXPECT_EQ(TBS::get_size_for_rb(100, 1), -1);

    // Невалидный RB
    EXPECT_EQ(TBS::get_size_for_rb(0, 0), -1);
    EXPECT_EQ(TBS::get_size_for_rb(0, 1000), -1);
    EXPECT_EQ(TBS::get_size_for_rb(1, 5), 22);
}

// Тест с неполными строками таблицы
TEST_F(TBSTest, ResourceOrder)
{
    auto table = TBS::get_table();

    for (size_t sub_table_idx = 0; sub_table_idx < table.size()-28; ++sub_table_idx){
        auto sub_table = table[sub_table_idx];

        for (size_t resource_idx = 1; resource_idx < sub_table.size() - 105; ++resource_idx){
            EXPECT_LE(sub_table[resource_idx - 1].first, sub_table[resource_idx].first);
            EXPECT_LT(sub_table[resource_idx - 1].second, sub_table[resource_idx].second);
        }
    }
}