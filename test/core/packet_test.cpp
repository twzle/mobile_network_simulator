#include "core/packet.hpp"
#include "core/user.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <climits>

class PacketTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_user = std::make_unique<User>(15, Position(), Mobility(), 100, 0.5);
    }

    std::unique_ptr<User> test_user;
    const int TEST_SIZE = 1500;
    const int TEST_QUEUE = 2;
    const double TEST_TIME = 123.45;
};

// Test constructor and basic getters
TEST_F(PacketTest, ConstructorAndBasicGetters) {
    Packet packet(TEST_QUEUE, TEST_SIZE, test_user.get());
    
    EXPECT_EQ(packet.get_size(), TEST_SIZE);
    EXPECT_EQ(packet.get_queue(), TEST_QUEUE);
    EXPECT_EQ(packet.get_user_ptr(), test_user.get());
    EXPECT_DOUBLE_EQ(packet.get_scheduled_at(), 0.0);
}

// Test size operations
TEST_F(PacketTest, SizeOperations) {
    Packet packet(TEST_QUEUE, TEST_SIZE, test_user.get());
    
    // Test initial size
    EXPECT_EQ(packet.get_size(), TEST_SIZE);
    
    // Test size modification
    packet.set_size(500);
    EXPECT_EQ(packet.get_size(), 500);
    
    // Test zero size
    packet.set_size(0);
    EXPECT_EQ(packet.get_size(), 0);
    
    // Test negative size (if allowed)
    packet.set_size(-100);
    EXPECT_EQ(packet.get_size(), -100);
}

// Test queue operations
TEST_F(PacketTest, QueueOperations) {
    Packet packet(TEST_QUEUE, TEST_SIZE, test_user.get());
    
    // Test initial queue
    EXPECT_EQ(packet.get_queue(), TEST_QUEUE);
    
    // Test queue modification
    packet.set_queue(1);
    EXPECT_EQ(packet.get_queue(), 1);
    
    // Test zero queue
    packet.set_queue(0);
    EXPECT_EQ(packet.get_queue(), 0);
    
    // Test negative queue (if allowed)
    packet.set_queue(-1);
    EXPECT_EQ(packet.get_queue(), -1);
}

// Test user pointer operations
TEST_F(PacketTest, UserPointerOperations) {
    Packet packet(TEST_QUEUE, TEST_SIZE, test_user.get());
    
    // Test initial user
    EXPECT_EQ(packet.get_user_ptr(), test_user.get());
    
    // Test setting new user
    User new_user(20, Position(), Mobility(), 100, 0.5);
    packet.set_user_ptr(&new_user);
    EXPECT_EQ(packet.get_user_ptr(), &new_user);
    
    // Test setting nullptr
    packet.set_user_ptr(nullptr);
    EXPECT_EQ(packet.get_user_ptr(), nullptr);
}

// Test scheduling time operations
TEST_F(PacketTest, SchedulingTimeOperations) {
    Packet packet(TEST_QUEUE, TEST_SIZE, test_user.get());
    
    // Test initial time
    EXPECT_DOUBLE_EQ(packet.get_scheduled_at(), 0.0);
    
    // Test setting time
    packet.set_scheduled_at(TEST_TIME);
    EXPECT_DOUBLE_EQ(packet.get_scheduled_at(), TEST_TIME);
    
    // Test negative time (if allowed)
    packet.set_scheduled_at(-10.0);
    EXPECT_DOUBLE_EQ(packet.get_scheduled_at(), -10.0);
}

// Test edge cases
TEST_F(PacketTest, EdgeCases) {
    // Maximum values
    Packet packet1(INT_MAX, INT_MAX, test_user.get());
    packet1.set_scheduled_at(DBL_MAX);
    
    EXPECT_EQ(packet1.get_size(), INT_MAX);
    EXPECT_EQ(packet1.get_queue(), INT_MAX);
    EXPECT_DOUBLE_EQ(packet1.get_scheduled_at(), DBL_MAX);
    
    // Minimum values
    Packet packet2(INT_MIN, INT_MIN, nullptr);
    packet2.set_scheduled_at(DBL_MIN);
    
    EXPECT_EQ(packet2.get_size(), INT_MIN);
    EXPECT_EQ(packet2.get_queue(), INT_MIN);
    EXPECT_DOUBLE_EQ(packet2.get_scheduled_at(), DBL_MIN);
}

// Test const correctness
TEST_F(PacketTest, ConstCorrectness) {
    const Packet const_packet(TEST_QUEUE, TEST_SIZE, test_user.get());
    
    // All getters should work with const object
    EXPECT_EQ(const_packet.get_size(), TEST_SIZE);
    EXPECT_EQ(const_packet.get_queue(), TEST_QUEUE);
    EXPECT_EQ(const_packet.get_user_ptr(), test_user.get());
    EXPECT_DOUBLE_EQ(const_packet.get_scheduled_at(), 0.0);
}