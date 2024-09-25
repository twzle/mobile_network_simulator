CXX = g++
CXXFLAGS = -O2 -Wall -Werror -Wextra -std=c++17 \
           -Iinclude \
           -I/usr/include/python3.10 \
		   -I./venv/include/python3.10 \
           -I./venv/lib/python3.10/site-packages/numpy/_core/include \
           -L/usr/lib/python3.10/config-3.10-x86_64-linux-gnu \
           -L/usr/lib/x86_64-linux-gnu \
           -lcrypt -ldl -lm -lpython3.10
           
LDFLAGS =

EXE = drr
SRCDIR = src
OBJDIR = obj

OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp))

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

include $(wildcard $(OBJDIR)/*.d)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(EXE)

.PHONY: clean all
