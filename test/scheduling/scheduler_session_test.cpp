#include "scheduling/scheduler_session.hpp"
#include <gtest/gtest.h>
#include <climits>


class SchedulerSessionTest : public ::testing::Test {
protected:
    SchedulerSession session;
    const double TEST_START_TIME = 100.0;
    const double TEST_END_TIME = 200.0;
    const int PACKET_INCREMENT = 5;
};

// Test initial state
TEST_F(SchedulerSessionTest, InitialState) {
    EXPECT_EQ(session.get_processed_packet_count(), 0);
    EXPECT_DOUBLE_EQ(session.get_scheduling_start_time(), 0.0);
    EXPECT_DOUBLE_EQ(session.get_scheduling_end_time(), 0.0);
}

// Test packet count increment
TEST_F(SchedulerSessionTest, PacketCountIncrement) {
    // Single increment
    session.increment_processed_packet_count(1);
    EXPECT_EQ(session.get_processed_packet_count(), 1);
    
    // Multiple increments
    session.increment_processed_packet_count(PACKET_INCREMENT);
    EXPECT_EQ(session.get_processed_packet_count(), 1 + PACKET_INCREMENT);
    
    // Negative increment (if allowed)
    session.increment_processed_packet_count(-2);
    EXPECT_EQ(session.get_processed_packet_count(), 1 + PACKET_INCREMENT - 2);
}

// Test time setting
TEST_F(SchedulerSessionTest, TimeSetting) {
    // Set start time
    session.set_scheduling_start_time(TEST_START_TIME);
    EXPECT_DOUBLE_EQ(session.get_scheduling_start_time(), TEST_START_TIME);
    
    // Set end time
    session.set_scheduling_end_time(TEST_END_TIME);
    EXPECT_DOUBLE_EQ(session.get_scheduling_end_time(), TEST_END_TIME);
    
    // Verify duration calculation
    double duration = TEST_END_TIME - TEST_START_TIME;
    EXPECT_DOUBLE_EQ(session.get_scheduling_end_time() - 
                   session.get_scheduling_start_time(), duration);
}

// Test edge cases
TEST_F(SchedulerSessionTest, EdgeCases) {
    // Maximum values
    session.increment_processed_packet_count(INT_MAX);
    EXPECT_EQ(session.get_processed_packet_count(), INT_MAX);
    
    session.set_scheduling_start_time(DBL_MAX);
    session.set_scheduling_end_time(DBL_MAX);
    EXPECT_DOUBLE_EQ(session.get_scheduling_start_time(), DBL_MAX);
    EXPECT_DOUBLE_EQ(session.get_scheduling_end_time(), DBL_MAX);
    
    // Minimum values
    SchedulerSession new_session;
    new_session.increment_processed_packet_count(INT_MIN);
    EXPECT_EQ(new_session.get_processed_packet_count(), INT_MIN);
    
    new_session.set_scheduling_start_time(DBL_MIN);
    new_session.set_scheduling_end_time(DBL_MIN);
    EXPECT_DOUBLE_EQ(new_session.get_scheduling_start_time(), DBL_MIN);
    EXPECT_DOUBLE_EQ(new_session.get_scheduling_end_time(), DBL_MIN);
}

// Test multiple operations
TEST_F(SchedulerSessionTest, MultipleOperations) {
    session.set_scheduling_start_time(TEST_START_TIME);
    session.increment_processed_packet_count(3);
    session.set_scheduling_end_time(TEST_END_TIME);
    session.increment_processed_packet_count(2);
    
    EXPECT_EQ(session.get_processed_packet_count(), 5);
    EXPECT_DOUBLE_EQ(session.get_scheduling_start_time(), TEST_START_TIME);
    EXPECT_DOUBLE_EQ(session.get_scheduling_end_time(), TEST_END_TIME);
}

// Test const correctness
TEST_F(SchedulerSessionTest, ConstCorrectness) {
    const SchedulerSession const_session;
    
    // All getters should work with const object
    EXPECT_EQ(const_session.get_processed_packet_count(), 0);
    EXPECT_DOUBLE_EQ(const_session.get_scheduling_start_time(), 0.0);
    EXPECT_DOUBLE_EQ(const_session.get_scheduling_end_time(), 0.0);
}