#include "core/packet_queue.hpp"
#include "core/packet.hpp"
#include "core/user.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <queue>

// Mock TimeGenerator for testing
class MockTimeGenerator {
public:
    static double generate_time() {
        return 123.45; // Fixed time for testing
    }
};

class PacketQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        User::reset_last_id();

        test_user = std::make_unique<User>(1, Position(), Mobility(), 100, 0.5);
        test_packet = std::make_unique<Packet>(0, 1500, test_user.get());
    }

    std::unique_ptr<User> test_user;
    std::unique_ptr<Packet> test_packet;
    const double TEST_QUANT = 0.5;
    const size_t TEST_LIMIT = 3;
};

// Test constructors
TEST_F(PacketQueueTest, Constructors) {
    // Default constructor
    PacketQueue pq1;
    EXPECT_DOUBLE_EQ(pq1.get_quant(), 0.0);
    EXPECT_DOUBLE_EQ(pq1.get_deficit(), 0.0);
    EXPECT_EQ(pq1.get_limit(), 0);

    // Parameterized constructor
    PacketQueue pq2(TEST_QUANT, TEST_LIMIT);
    EXPECT_DOUBLE_EQ(pq2.get_quant(), TEST_QUANT);
    EXPECT_DOUBLE_EQ(pq2.get_deficit(), 0.0);
    EXPECT_EQ(pq2.get_limit(), TEST_LIMIT);
}

// Test packet scheduling
TEST_F(PacketQueueTest, SchedulePacket) {
    PacketQueue pq(TEST_QUANT, TEST_LIMIT);
    
    // Schedule packets up to limit
    for (size_t i = 0; i < TEST_LIMIT; ++i) {
        Packet p(i, 1000 + i*100, test_user.get());
        pq.schedule_packet(p);
        EXPECT_EQ(pq.size(), i + 1);
    }

    // Try to schedule beyond limit
    Packet extra_packet(0, 2000, test_user.get());
    pq.schedule_packet(extra_packet);
    EXPECT_EQ(pq.size(), TEST_LIMIT); // Size shouldn't increase
}

// Test deficit operations
TEST_F(PacketQueueTest, DeficitOperations) {
    PacketQueue pq;
    
    // Test setting valid deficit
    pq.set_deficit(50.0);
    EXPECT_DOUBLE_EQ(pq.get_deficit(), 50.0);
    
    // Test setting maximum deficit
    pq.set_deficit(DEFICIT_MAX + 10.0);
    EXPECT_DOUBLE_EQ(pq.get_deficit(), DEFICIT_MAX);
    
    // Test setting negative deficit (if allowed)
    pq.set_deficit(-10.0);
    EXPECT_DOUBLE_EQ(pq.get_deficit(), -10.0);
}

// Test queue operations
TEST_F(PacketQueueTest, QueueOperations) {
    PacketQueue pq(TEST_QUANT, TEST_LIMIT);
    
    // Test empty queue
    EXPECT_TRUE(pq.size() == 0);
    
    // Push packets with different scheduling times
    Packet p1(0, 1000, test_user.get());
    p1.set_scheduled_at(10.0);
    pq.push(p1);
    
    Packet p2(0, 1500, test_user.get());
    p2.set_scheduled_at(5.0);
    pq.push(p2);
    
    // Verify ordering (should be by scheduled time)
    EXPECT_DOUBLE_EQ(pq.front().get_scheduled_at(), 5.0);
    pq.pop();
    EXPECT_DOUBLE_EQ(pq.front().get_scheduled_at(), 10.0);
    pq.pop();
    EXPECT_TRUE(pq.size() == 0);
}

// Test edge cases
TEST_F(PacketQueueTest, EdgeCases) {
    // Maximum values
    PacketQueue pq1(DBL_MAX, SIZE_MAX);
    pq1.set_deficit(DBL_MAX);
    
    EXPECT_DOUBLE_EQ(pq1.get_quant(), DBL_MAX);
    EXPECT_EQ(pq1.get_limit(), SIZE_MAX);
    EXPECT_DOUBLE_EQ(pq1.get_deficit(), DEFICIT_MAX); // Should be capped
    
    // Minimum values
    PacketQueue pq2(DBL_MIN, 0);
    pq2.set_deficit(DBL_MIN);
    
    EXPECT_DOUBLE_EQ(pq2.get_quant(), DBL_MIN);
    EXPECT_EQ(pq2.get_limit(), 0);
    EXPECT_DOUBLE_EQ(pq2.get_deficit(), DBL_MIN);
}

// Test print method (output capture)
TEST_F(PacketQueueTest, PrintMethod) {
    PacketQueue pq(TEST_QUANT, TEST_LIMIT);
    
    // Add test packets
    Packet p1(0, 1000, test_user.get());
    p1.set_scheduled_at(10.0);
    pq.push(p1);
    
    Packet p2(1, 1500, test_user.get());
    p2.set_scheduled_at(5.0);
    pq.push(p2);
    
    // Redirect cout to stringstream for testing
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    pq.print();
    
    std::cout.rdbuf(old); // Reset cout
    
    std::string output = buffer.str();

    EXPECT_TRUE(output.find("SCHEDULE TIME = 5") != std::string::npos);
    EXPECT_TRUE(output.find("SCHEDULE TIME = 10") != std::string::npos);
    EXPECT_TRUE(output.find("USER = 0") != std::string::npos);
}

// Test const correctness
TEST_F(PacketQueueTest, ConstCorrectness) {
    const PacketQueue pq(TEST_QUANT, TEST_LIMIT);
    
    // All getters should work with const object
    EXPECT_DOUBLE_EQ(pq.get_quant(), TEST_QUANT);
    EXPECT_DOUBLE_EQ(pq.get_deficit(), 0.0);
    EXPECT_EQ(pq.get_limit(), TEST_LIMIT);
    EXPECT_EQ(pq.size(), 0);
}