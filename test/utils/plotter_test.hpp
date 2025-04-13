#ifndef PLOTTER_TEST_HPP
#define PLOTTER_TEST_HPP

#include "utils/plotter.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <string>

// Константы для тестов
extern const std::string TEST_FILE;
extern const std::string TEST_PLOTTER_PATH;

/**
 * @brief Фикстура для тестирования класса Plotter
 */
class PlotterTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
};

#endif // PLOTTER_TEST_HPP