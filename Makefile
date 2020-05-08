
CXX = g++
CXXFLAGS = -g -I. -I ./Midi
WARNING = -Wall -Wextra
BUILDDIR = build
GTK3FLAGS = `pkg-config --cflags gtk+-3.0`
GTK3LIBS = `pkg-config --libs gtk+-3.0`
OUT_DIR = $(BUILDDIR) $(BUILDDIR)/Midi
SRC = $(wildcard *.cpp) $(wildcard Midi/*.cpp)
DEPS = $(patsubst %.cpp,%.d,$(SRC))
OBJ = $(SRC:%.cpp=$(BUILDDIR)/%.o)

.PHONY: all clean a.out

all: directories a.out

a.out: $(OBJ)
	$(CXX) $(WARNING) -o $@ $^ $(CXXFLAGS) $(GTK3FLAGS) $(GTK3LIBS)

-include $(DEPS)

$(BUILDDIR)/%.o: ./%.cpp
	$(CXX) $(WARNING) $(CXXFLAGS) $(GTK3FLAGS) -c $< -o $@ -MMD -MP $(GTK3LIBS)

directories: ${OUT_DIR}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

clean:
	rm -rf build
	rm -f a.out