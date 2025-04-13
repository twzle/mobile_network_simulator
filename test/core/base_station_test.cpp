#include "core/base_station.hpp"
#include "core/position.hpp"
#include <gtest/gtest.h>

class BaseStationTest : public ::testing::Test {
protected:
    const Position TEST_POSITION = Position(10.5, 20.5, 30.5);
    const Position DEFAULT_POSITION = Position(0.0, 0.0, 0.0);
    const double DELTA = 1e-9;
};

// Test default constructor
TEST_F(BaseStationTest, DefaultConstructor) {
    BaseStation bs;
    Position pos = bs.get_position();
    
    EXPECT_DOUBLE_EQ(pos.get_x(), DEFAULT_POSITION.get_x());
    EXPECT_DOUBLE_EQ(pos.get_y(), DEFAULT_POSITION.get_y());
    EXPECT_DOUBLE_EQ(pos.get_z(), DEFAULT_POSITION.get_z());
}

// Test parameterized constructor
TEST_F(BaseStationTest, ParameterizedConstructor) {
    BaseStation bs(TEST_POSITION);
    Position pos = bs.get_position();
    
    EXPECT_DOUBLE_EQ(pos.get_x(), TEST_POSITION.get_x());
    EXPECT_DOUBLE_EQ(pos.get_y(), TEST_POSITION.get_y());
    EXPECT_DOUBLE_EQ(pos.get_z(), TEST_POSITION.get_z());
}

// Test position setter and getter
TEST_F(BaseStationTest, PositionSetterGetter) {
    BaseStation bs;
    
    // Initial position should be default
    Position initialPos = bs.get_position();
    EXPECT_DOUBLE_EQ(initialPos.get_x(), 0.0);
    
    // Set new position
    bs.set_position(TEST_POSITION);
    Position newPos = bs.get_position();
    
    EXPECT_DOUBLE_EQ(newPos.get_x(), TEST_POSITION.get_x());
    EXPECT_DOUBLE_EQ(newPos.get_y(), TEST_POSITION.get_y());
    EXPECT_DOUBLE_EQ(newPos.get_z(), TEST_POSITION.get_z());
    
    // Test setting to negative coordinates
    Position negativePos(-10.0, -20.0, -30.0);
    bs.set_position(negativePos);
    Position retrievedPos = bs.get_position();
    
    EXPECT_DOUBLE_EQ(retrievedPos.get_x(), negativePos.get_x());
    EXPECT_DOUBLE_EQ(retrievedPos.get_y(), negativePos.get_y());
    EXPECT_DOUBLE_EQ(retrievedPos.get_z(), negativePos.get_z());
}

// Test position modification
TEST_F(BaseStationTest, PositionModification) {
    BaseStation bs(TEST_POSITION);
    
    // Modify position through getter reference
    Position pos = bs.get_position();
    pos.set_x(100.0);
    
    // Original position shouldn't change (getter returns copy)
    EXPECT_DOUBLE_EQ(bs.get_position().get_x(), TEST_POSITION.get_x());
    
    // Explicitly set modified position
    bs.set_position(pos);
    EXPECT_DOUBLE_EQ(bs.get_position().get_x(), 100.0);
}

// Test edge cases
TEST_F(BaseStationTest, EdgeCases) {
    // Maximum coordinate values
    Position maxPos(DBL_MAX, DBL_MAX, DBL_MAX);
    BaseStation bs1(maxPos);
    Position retrievedMax = bs1.get_position();
    
    EXPECT_DOUBLE_EQ(retrievedMax.get_x(), DBL_MAX);
    EXPECT_DOUBLE_EQ(retrievedMax.get_y(), DBL_MAX);
    EXPECT_DOUBLE_EQ(retrievedMax.get_z(), DBL_MAX);
    
    // Minimum coordinate values
    Position minPos(DBL_MIN, DBL_MIN, DBL_MIN);
    BaseStation bs2(minPos);
    Position retrievedMin = bs2.get_position();
    
    EXPECT_DOUBLE_EQ(retrievedMin.get_x(), DBL_MIN);
    EXPECT_DOUBLE_EQ(retrievedMin.get_y(), DBL_MIN);
    EXPECT_DOUBLE_EQ(retrievedMin.get_z(), DBL_MIN);
}

// Test const correctness
TEST_F(BaseStationTest, ConstCorrectness) {
    const BaseStation constBs(TEST_POSITION);
    Position pos = constBs.get_position(); // Should work with const object
    
    EXPECT_DOUBLE_EQ(pos.get_x(), TEST_POSITION.get_x());
    
    // Verify get_position is really const
    static_assert(std::is_const_v<std::remove_reference_t<
        decltype(constBs.get_position())>> == false,
        "get_position should return non-const copy");
}