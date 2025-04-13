#include "core/mobility.hpp"
#include <gtest/gtest.h>
#include <string>

class MobilityTest : public ::testing::Test {
protected:
    const double TEST_SPEED = 30.5;
    const std::string TEST_DIRECTION = "north";
};

// Test default constructor
TEST_F(MobilityTest, DefaultConstructor) {
    Mobility mob;
    EXPECT_DOUBLE_EQ(mob.get_speed(), 0.0);
    EXPECT_EQ(mob.get_direction(), "");
}

// Test parameterized constructor
TEST_F(MobilityTest, ParameterizedConstructor) {
    Mobility mob(TEST_SPEED, TEST_DIRECTION);
    EXPECT_DOUBLE_EQ(mob.get_speed(), TEST_SPEED);
    EXPECT_EQ(mob.get_direction(), TEST_DIRECTION);
}

// Test copy constructor
TEST_F(MobilityTest, CopyConstructor) {
    Mobility original(TEST_SPEED, TEST_DIRECTION);
    Mobility copy(original);
    
    EXPECT_DOUBLE_EQ(copy.get_speed(), TEST_SPEED);
    EXPECT_EQ(copy.get_direction(), TEST_DIRECTION);
}

// Test assignment operator
TEST_F(MobilityTest, AssignmentOperator) {
    Mobility original(TEST_SPEED, TEST_DIRECTION);
    Mobility assigned;
    
    assigned = original;
    
    EXPECT_DOUBLE_EQ(assigned.get_speed(), TEST_SPEED);
    EXPECT_EQ(assigned.get_direction(), TEST_DIRECTION);
    
    // Test self-assignment
    assigned = assigned;
    EXPECT_DOUBLE_EQ(assigned.get_speed(), TEST_SPEED);
}

// Test speed getter/setter
TEST_F(MobilityTest, SpeedOperations) {
    Mobility mob;
    
    // Test default speed
    EXPECT_DOUBLE_EQ(mob.get_speed(), 0.0);
    
    // Test setting positive speed
    Mobility mob1(50.0, "east");
    EXPECT_DOUBLE_EQ(mob1.get_speed(), 50.0);
    
    // Test setting zero speed
    Mobility mob2(0.0, "west");
    EXPECT_DOUBLE_EQ(mob2.get_speed(), 0.0);
    
    // Test setting negative speed (if allowed)
    Mobility mob3(-10.0, "south");
    EXPECT_DOUBLE_EQ(mob3.get_speed(), -10.0);
}

// Test direction getter/setter
TEST_F(MobilityTest, DirectionOperations) {
    Mobility mob;
    
    // Test default direction
    EXPECT_EQ(mob.get_direction(), "");
    
    // Test setting standard directions
    Mobility mob1(10.0, "north");
    EXPECT_EQ(mob1.get_direction(), "north");
    
    Mobility mob2(20.0, "south");
    EXPECT_EQ(mob2.get_direction(), "south");
    
    Mobility mob3(30.0, "east");
    EXPECT_EQ(mob3.get_direction(), "east");
    
    Mobility mob4(40.0, "west");
    EXPECT_EQ(mob4.get_direction(), "west");
    
    // Test setting custom direction
    Mobility mob5(50.0, "north-east");
    EXPECT_EQ(mob5.get_direction(), "north-east");
    
    // Test setting empty direction
    Mobility mob6(60.0, "");
    EXPECT_EQ(mob6.get_direction(), "");
}

// Test edge cases
TEST_F(MobilityTest, EdgeCases) {
    // Maximum speed value
    Mobility mob1(DBL_MAX, "up");
    EXPECT_DOUBLE_EQ(mob1.get_speed(), DBL_MAX);
    
    // Minimum speed value
    Mobility mob2(DBL_MIN, "down");
    EXPECT_DOUBLE_EQ(mob2.get_speed(), DBL_MIN);
    
    // Long direction string
    std::string long_dir(1000, 'n');
    Mobility mob3(10.0, long_dir);
    EXPECT_EQ(mob3.get_direction(), long_dir);
}

// Test move semantics (if implemented)
TEST_F(MobilityTest, MoveOperations) {
    Mobility original(TEST_SPEED, TEST_DIRECTION);
    Mobility moved(std::move(original));
    
    EXPECT_DOUBLE_EQ(moved.get_speed(), TEST_SPEED);
    EXPECT_EQ(moved.get_direction(), TEST_DIRECTION);
    
    // Original should be in valid but unspecified state
    // (implementation dependent)
}