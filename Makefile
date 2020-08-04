
CXX = g++
CXXFLAGS = -g -I. -I ./Midi -I ./GUI
WARNING = -Wall -Wextra
BUILDDIR = build
GTK3MMFLAGS = `pkg-config gtkmm-3.0 --cflags --libs`
OUT_DIR = $(BUILDDIR) $(BUILDDIR)/Midi $(BUILDDIR)/GUI
SRC = $(wildcard *.cpp) $(wildcard Midi/*.cpp) $(wildcard GUI/*.cpp)
DEPS = $(patsubst %.cpp,%.d,$(SRC))
OBJ = $(SRC:%.cpp=$(BUILDDIR)/%.o)

.PHONY: all clean a.out

all: directories a.out

a.out: $(OBJ)
	$(CXX) $(WARNING) -o $@ $^ $(CXXFLAGS) $(GTK3MMFLAGS)

-include $(DEPS)

$(BUILDDIR)/%.o: ./%.cpp
	$(CXX) $(WARNING) $(CXXFLAGS) $(GTK3MMFLAGS) -c $< -o $@ -MMD -MP

directories: ${OUT_DIR}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

clean:
	rm -rf build
	rm -f a.out

rebuild: clean all