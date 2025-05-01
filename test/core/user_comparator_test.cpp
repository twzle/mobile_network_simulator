#include "core/user.hpp"
#include <gtest/gtest.h>
#include <queue>

class UserPFComparatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        User::initialize(); // Сброс статического last_id
        user1 = new User(10, Position(0, 0, 0), Mobility(30.0, "forward"), 5, 1.0);
        user2 = new User(12, Position(1, 1, 1), Mobility(50.0, "backward"), 5, 1.0);
        user3 = new User(8, Position(2, 2, 2), Mobility(20.0, "forward"), 5, 1.0);
    }

    void TearDown() override {
        delete user1;
        delete user2;
        delete user3;
    }

    User* user1;
    User* user2;
    User* user3;
};

TEST_F(UserPFComparatorTest, CompareByPriority) {
    UserPFComparator comparator;

    // Устанавливаем разные приоритеты
    user1->set_priority(0.8);
    user2->set_priority(0.5);
    user3->set_priority(0.8);

    // Проверяем, что пользователь с меньшим приоритетом идет первым
    EXPECT_FALSE(comparator(user2, user1)); // 0.8 > 0.5 -> false

    // При равных приоритетах сравниваем по throughput
    user1->set_current_throughput(100);
    user1->update_throughput_history(); // average = 100
    user3->set_current_throughput(200);
    user3->update_throughput_history(); // average = 200

    // user1 должен быть выше, т.к. throughput меньше (100 < 200)
    EXPECT_FALSE(comparator(user3, user1));
}

TEST_F(UserPFComparatorTest, PriorityQueueOrder) {
    std::priority_queue<User*, std::vector<User*>, UserPFComparator> queue;

    // Настраиваем пользователей
    user1->set_priority(0.7);
    user1->set_current_throughput(150);
    user1->update_throughput_history();

    user2->set_priority(0.9);
    user2->set_current_throughput(300);
    user2->update_throughput_history();

    user3->set_priority(0.7);
    user3->set_current_throughput(100);
    user3->update_throughput_history();

    // Добавляем в очередь (порядок не важен)
    queue.push(user1);
    queue.push(user2);
    queue.push(user3);

    // Проверяем порядок извлечения
    EXPECT_EQ(queue.top(), user1);
    queue.pop();

    EXPECT_EQ(queue.top(), user3);
    queue.pop();

    EXPECT_EQ(queue.top(), user2);
}