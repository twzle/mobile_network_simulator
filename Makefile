CXX = g++
CXXFLAGS = -fprofile-arcs -ftest-coverage -Wall -Werror -Wextra -std=c++17 -Iinclude
TEST_CXXFLAGS = $(CXXFLAGS)
LDFLAGS = -lyaml-cpp
TEST_LDFLAGS = -lgtest -lgtest_main -lpthread --coverage  

EXE = simulator
TEST_EXE = test_runner
SRCDIRS = src scenarios/cpp
TESTDIR = test
OBJDIR = obj
TESTOBJDIR = $(OBJDIR)/test

SOURCES = $(shell find $(SRCDIRS) -name '*.cpp')
TEST_SOURCES = $(shell find $(TESTDIR) -name '*.cpp')

OBJECTS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(SOURCES))
TEST_OBJECTS = $(patsubst %.cpp, $(TESTOBJDIR)/%.o, $(TEST_SOURCES))

all: $(EXE)

test: $(TEST_EXE)
	./$(TEST_EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(CXXFLAGS) $(LDFLAGS)

$(TEST_EXE): $(filter-out $(OBJDIR)/src/main.o, $(OBJECTS)) $(TEST_OBJECTS)
	$(CXX) $^ -o $@ $(TEST_CXXFLAGS) $(LDFLAGS) $(TEST_LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

$(TESTOBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_CXXFLAGS) -I$(TESTDIR) -c -MMD -o $@ $< 

-include $(OBJECTS:.o=.d)
-include $(TEST_OBJECTS:.o=.d)

clean:
	rm -rf $(OBJDIR) $(EXE) $(TEST_EXE) *.gcda *.gcno coverage.info coverage_report

.PHONY: clean all test