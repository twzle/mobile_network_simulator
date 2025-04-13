#include "core/user.hpp"
#include "core/position.hpp"
#include "core/mobility.hpp"
#include <gtest/gtest.h>
#include <cmath>

class UserTest : public ::testing::Test {
protected:
    void SetUp() override {
        User::initialize();
        test_position = Position(10.0, 20.0, 0.0);
        test_mobility = Mobility(30.0, "forward");
    }

    Position test_position;
    Mobility test_mobility;
    const double DELTA = 1e-9;
};

// Тест конструктора и базовых геттеров
TEST_F(UserTest, ConstructorAndBasicGetters) {
    User user(15, test_position, test_mobility, 100, 0.5);
    
    EXPECT_EQ(user.get_id(), 0);
    EXPECT_EQ(user.get_cqi(), 15);
    EXPECT_DOUBLE_EQ(user.get_position().get_x(), 10.0);
    EXPECT_DOUBLE_EQ(user.get_mobility().get_speed(), 30.0);
    EXPECT_DOUBLE_EQ(user.get_quant(), 0.5);
    EXPECT_DOUBLE_EQ(user.get_average_throughput(), 0.1);
}

// Тест последовательных ID пользователей
TEST_F(UserTest, SequentialUserIds) {
    User user1(10, test_position, test_mobility, 100, 0.5);
    User user2(10, test_position, test_mobility, 100, 0.5);
    User user3(10, test_position, test_mobility, 100, 0.5);
    
    EXPECT_EQ(user1.get_id(), 0);
    EXPECT_EQ(user2.get_id(), 1);
    EXPECT_EQ(user3.get_id(), 2);
    
    User::reset_last_id();
    User user4(10, test_position, test_mobility, 100, 0.5);
    EXPECT_EQ(user4.get_id(), 0);
}

// Тест работы с CQI
TEST_F(UserTest, CqiOperations) {
    User user(10, test_position, test_mobility, 100, 0.5);
    
    user.set_cqi(20);
    EXPECT_EQ(user.get_cqi(), 20);
    
    user.set_cqi(0);
    EXPECT_EQ(user.get_cqi(), 0);
    
    user.set_cqi(255);
    EXPECT_EQ(user.get_cqi(), 255);
}

// Тест приоритета
TEST_F(UserTest, PriorityOperations) {
    User user(10, test_position, test_mobility, 100, 0.5);
    
    user.set_priority(1.5);
    EXPECT_DOUBLE_EQ(user.get_priority(), 1.5);
    
    user.set_priority(0.0);
    EXPECT_DOUBLE_EQ(user.get_priority(), 0.0);
    
    user.set_priority(-1.0);
    EXPECT_DOUBLE_EQ(user.get_priority(), -1.0);
}

// Тест кандидата на ресурсы
TEST_F(UserTest, ResourceCandidateFlag) {
    User user(10, test_position, test_mobility, 100, 0.5);
    
    EXPECT_FALSE(user.is_resource_candidate());
    
    user.set_resource_candidate(true);
    EXPECT_TRUE(user.is_resource_candidate());
    
    user.set_resource_candidate(false);
    EXPECT_FALSE(user.is_resource_candidate());
}

// Тест throughput
TEST_F(UserTest, ThroughputOperations) {
    const int history_size = 10;
    User user(10, test_position, test_mobility, history_size, 0.5);
    
    // Проверка начального значения
    EXPECT_DOUBLE_EQ(user.get_average_throughput(), 0.1);
    
    // Установка текущего throughput
    user.set_current_throughput(100.0);
    
    // Обновление истории
    user.update_throughput_history();
    
    // Точный расчет:
    // alpha = 2/(N+1) = 2/11 ≈ 0.18181818
    // new_avg = (1-alpha)*old_avg + alpha*current
    //         = (1-0.18181818)*0.1 + 0.18181818*100
    //         = 0.81818182*0.1 + 18.181818
    //         = 0.081818182 + 18.181818 ≈ 18.263636
    EXPECT_NEAR(user.get_average_throughput(), 18.263636363636365, DELTA);
    
    // Проверка инкремента
    const int rb_count = 5;
    user.increment_current_throughput(rb_count);
    // Cогласно TBS для CQI 10 и 5 ресурсных блоков, пользователь получит:
    // 193 байт пропускной способности, суммарно 100 + 193 = 293 байт
    EXPECT_NEAR(user.get_current_throughput(), 100.0 + 193, DELTA);
}

// Тест движения пользователя
TEST_F(UserTest, UserMovement) {
    Position start_pos(0.0, 100.0, 0.0);
    Mobility mobility(36.0, "forward"); // 36 km/h = 10 m/s
    User user(10, start_pos, mobility, 100, 0.5);
    
    // Движение на 1 секунду
    user.move(1.0);
    Position new_pos = user.get_position();
    
    // Должен переместиться на 10 метров по Y
    EXPECT_NEAR(new_pos.get_y(), 110.0, DELTA);
    EXPECT_NEAR(new_pos.get_x(), 0.0, DELTA);
    
    // Проверка граничных условий
    Position edge_pos(0.0, BS_TO_UE_DISTANCE_MAX - 1, 0.0);
    User edge_user(10, edge_pos, mobility, 100, 0.5);
    edge_user.move(1.0);
    EXPECT_NEAR(edge_user.get_position().get_y(), BS_TO_UE_DISTANCE_MAX, DELTA);
}

// Тест времени синхронизации канала
TEST_F(UserTest, ChannelSyncTime) {
    User user(10, test_position, test_mobility, 100, 0.5);
    
    EXPECT_DOUBLE_EQ(user.get_time_from_last_channel_sync(), 0.0);
    
    user.set_time_from_last_channel_sync(1.5);
    EXPECT_DOUBLE_EQ(user.get_time_from_last_channel_sync(), 1.5);
    
    user.set_time_from_last_channel_sync(0.0);
    EXPECT_DOUBLE_EQ(user.get_time_from_last_channel_sync(), 0.0);
}

// Тест дефицита
TEST_F(UserTest, DeficitOperations) {
    User user(10, test_position, test_mobility, 100, 0.5);
    
    user.set_deficit(1.5);
    EXPECT_DOUBLE_EQ(user.get_deficit(), 1.5);
    
    user.set_deficit(0.0);
    EXPECT_DOUBLE_EQ(user.get_deficit(), 0.0);
    
    user.set_deficit(-1.0);
    EXPECT_DOUBLE_EQ(user.get_deficit(), -1.0);
    
    user.set_deficit(DEFICIT_MAX + 1.0);
    EXPECT_DOUBLE_EQ(user.get_deficit(), DEFICIT_MAX);
}