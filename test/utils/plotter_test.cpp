#include "utils/plotter_test.hpp"
#include <fstream>

const std::string TEST_FILE = "test_data.txt";
const std::string TEST_PLOTTER_PATH = "../Plotter/";

// Реализация методов фикстуры
void PlotterTest::SetUp() {
    std::ofstream(TEST_FILE) << "test data";
}

void PlotterTest::TearDown() {
    std::filesystem::remove(TEST_FILE);
}

// Реализации тестов (используем макросы ТОЛЬКО здесь)
TEST_F(PlotterTest, SuccessfulRun) {
    Plotter plotter(TEST_PLOTTER_PATH);
    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(plotter.run(TEST_FILE));
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(output.empty());
}

TEST_F(PlotterTest, InvalidPlotterPath) {
    Plotter plotter("nonexistent_directory");
    testing::internal::CaptureStderr();
    plotter.run(TEST_FILE);
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_FALSE(output.empty());
}