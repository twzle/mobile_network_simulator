CXX = g++
CXXFLAGS = -O2 -Wall -Werror -Wextra -std=c++17 -Iinclude
LDFLAGS = -lyaml-cpp

EXE = drr
SRCDIRS = src scenarios/cpp
OBJDIR = obj

# Ищем все .cpp файлы в src и scenarios/cpp
SOURCES = $(shell find $(SRCDIRS) -name '*.cpp')

# Преобразуем пути из SRCDIRS/*.cpp в OBJDIR/*.o
OBJECTS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(SOURCES))

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@) # Создаем вложенные папки в obj/ при необходимости
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(OBJDIR) $(EXE)

.PHONY: clean all
