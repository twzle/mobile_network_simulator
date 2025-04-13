#include "core/relevant_packet_queue.hpp"
#include "core/packet.hpp"
#include "core/user.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <sstream>

class RelevantPacketQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        User::reset_last_id();

        user1 = std::make_shared<User>(1, Position(), Mobility(), 100, 0.5);
        user2 = std::make_shared<User>(2, Position(), Mobility(), 100, 0.7);
        
        // Set different priorities
        user1->set_priority(1.0);
        user2->set_priority(2.0); // Higher priority
        
        packet1 = std::make_unique<Packet>(0, 1000, user1.get());
        packet2 = std::make_unique<Packet>(1, 1500, user2.get());
        
        // Set different scheduling times
        packet1->set_scheduled_at(10.0);
        packet2->set_scheduled_at(5.0);
    }

    std::shared_ptr<User> user1;
    std::shared_ptr<User> user2;
    std::unique_ptr<Packet> packet1;
    std::unique_ptr<Packet> packet2;
};

// Test default constructor
TEST_F(RelevantPacketQueueTest, DefaultConstructor) {
    RelevantPacketQueue rpq;
    EXPECT_EQ(rpq.size(), 0);
}

// Test push and size
TEST_F(RelevantPacketQueueTest, PushAndSize) {
    RelevantPacketQueue rpq;
    
    rpq.push(*packet1);
    EXPECT_EQ(rpq.size(), 1);
    
    rpq.push(*packet2);
    EXPECT_EQ(rpq.size(), 2);
}

// Test queue ordering (priority then time)
TEST_F(RelevantPacketQueueTest, QueueOrdering) {
    RelevantPacketQueue rpq;
    
    // Higher priority packet should come first regardless of time
    rpq.push(*packet1); // priority 1.0, time 10.0
    rpq.push(*packet2); // priority 2.0, time 5.0
    
    // Higher priority (2.0) should come first
    EXPECT_FLOAT_EQ(rpq.front().get_user_ptr()->get_priority(), 2.0);
    rpq.pop();
    
    // Then lower priority (1.0)
    EXPECT_FLOAT_EQ(rpq.front().get_user_ptr()->get_priority(), 1.0);
}

// Test equal priority ordering (should use time)
TEST_F(RelevantPacketQueueTest, EqualPriorityOrdering) {
    RelevantPacketQueue rpq;
    
    // Set same priority
    user1->set_priority(1.0);
    user2->set_priority(1.0);
    
    rpq.push(*packet1); // time 10.0
    rpq.push(*packet2); // time 5.0
    
    // Earlier time (5.0) should come first
    EXPECT_DOUBLE_EQ(rpq.front().get_scheduled_at(), 5.0);
    rpq.pop();
    
    // Then later time (10.0)
    EXPECT_DOUBLE_EQ(rpq.front().get_scheduled_at(), 10.0);
}

// Test front and pop operations
TEST_F(RelevantPacketQueueTest, FrontAndPop) {
    RelevantPacketQueue rpq;

    rpq.push(*packet1);
    rpq.push(*packet2);
    
    Packet front = rpq.front();
    EXPECT_EQ(front.get_user_ptr()->get_id(), 1); // Higher priority user
    
    rpq.pop();
    EXPECT_EQ(rpq.size(), 1);
    
    front = rpq.front();
    EXPECT_EQ(front.get_user_ptr()->get_id(), 0);
}

// Test print method output
TEST_F(RelevantPacketQueueTest, PrintOutput) {
    RelevantPacketQueue rpq;
    rpq.push(*packet1); // USER = 0, time = 10.0
    rpq.push(*packet2); // USER = 1, time = 5.0
    
    // Redirect cout to stringstream
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    rpq.print();
    
    std::cout.rdbuf(old); // Reset cout
    
    std::string output = buffer.str();
    
    // Проверяем наличие всех ожидаемых данных в выводе
    // Не проверяем порядок, так как он зависит от компаратора
    EXPECT_TRUE(output.find("USER = 0") != std::string::npos);
    EXPECT_TRUE(output.find("USER = 1") != std::string::npos);
    EXPECT_TRUE(output.find("SCHEDULE TIME = 10") != std::string::npos);
    EXPECT_TRUE(output.find("SCHEDULE TIME = 5") != std::string::npos);
    
    // Дополнительно можно проверить формат вывода
    EXPECT_TRUE(output.find("PACKET = 0") != std::string::npos);
    EXPECT_TRUE(output.find("PACKET = 1") != std::string::npos);
}

// Test const correctness
TEST_F(RelevantPacketQueueTest, ConstCorrectness) {
    RelevantPacketQueue rpq;
    rpq.push(*packet1);
    
    const RelevantPacketQueue& const_rpq = rpq;
    
    // Should work with const object
    EXPECT_EQ(const_rpq.size(), 1);
    Packet p = const_rpq.front(); // front() should be const
    EXPECT_EQ(p.get_user_ptr()->get_id(), 0);
}