CXX = g++
CXXFLAGS = -O2 -Wall -Werror -Wextra -std=c++17 -Iinclude
LDFLAGS = -lyaml-cpp

EXE = drr
SRCDIR = src
OBJDIR = obj


SOURCES = $(shell find $(SRCDIR) -name '*.cpp')

OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@) # Создаем вложенные папки в obj/ при необходимости
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(OBJDIR) $(EXE)

.PHONY: clean all
