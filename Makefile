CXX = g++
CXXFLAGS = -O2 -Wall -Werror -Wextra -std=c++17 -Iinclude
LDFLAGS = -lyaml-cpp
TEST_LDFLAGS = -lgtest -lgtest_main -lpthread

EXE = drr
TEST_EXE = test_runner
SRCDIRS = src scenarios/cpp
TESTDIR = test
OBJDIR = obj
TESTOBJDIR = $(OBJDIR)/test

# Ищем все .cpp файлы в src и scenarios/cpp
SOURCES = $(shell find $(SRCDIRS) -name '*.cpp')
# Ищем все тестовые .cpp файлы
TEST_SOURCES = $(shell find $(TESTDIR) -name '*.cpp')

# Преобразуем пути из SRCDIRS/*.cpp в OBJDIR/*.o
OBJECTS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(SOURCES))
# Преобразуем пути из TESTDIR/*.cpp в TESTOBJDIR/*.o
TEST_OBJECTS = $(patsubst %.cpp, $(TESTOBJDIR)/%.o, $(TEST_SOURCES))

all: $(EXE)

test: $(TEST_EXE)
	./$(TEST_EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(CXXFLAGS) $(LDFLAGS)

$(TEST_EXE): $(filter-out $(OBJDIR)/src/main.o, $(OBJECTS)) $(TEST_OBJECTS)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS) $(TEST_LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@) # Создаем вложенные папки в obj/ при необходимости
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

$(TESTOBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@) # Создаем вложенные папки в obj/test/ при необходимости
	$(CXX) $(CXXFLAGS) -I$(TESTDIR) -c -MMD -o $@ $<

-include $(OBJECTS:.o=.d)
-include $(TEST_OBJECTS:.o=.d)

clean:
	rm -rf $(OBJDIR) $(EXE) $(TEST_EXE)

.PHONY: clean all test