cmake_minimum_required(VERSION 3.12)
project(Simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -Wall -Werror -Wextra")

# Исходные файлы
file(GLOB_RECURSE MAIN_SOURCES CONFIGURE_DEPENDS src/*.cpp scenarios/cpp/*.cpp)
list(REMOVE_ITEM MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)

# Тесты
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS test/*.cpp)
file(GLOB_RECURSE TESTABLE_SOURCES CONFIGURE_DEPENDS src/*.cpp scenarios/cpp/*.cpp)
list(REMOVE_ITEM TESTABLE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(test_runner ${TEST_SOURCES} ${TESTABLE_SOURCES})
target_include_directories(test_runner PRIVATE include test)
target_link_libraries(test_runner PRIVATE gtest gtest_main pthread yaml-cpp)
set_target_properties(test_runner PROPERTIES 
    COMPILE_FLAGS "--coverage"
    LINK_FLAGS "--coverage"
)

add_custom_target(run_test COMMAND ./test_runner DEPENDS test_runner)
enable_testing()
add_test(NAME tests COMMAND test_runner)

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E rm -f *.gcda *.gcno coverage.info
    COMMAND ${CMAKE_COMMAND} -E remove_directory coverage_report
)