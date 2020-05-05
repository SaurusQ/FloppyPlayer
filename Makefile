
CXX = g++
CXXFLAGS = -g -I. -I ./Midi
WARNING = -Wall -Wextra
BUILDDIR = build
OUT_DIR = $(BUILDDIR) $(BUILDDIR)/Midi
SRC = $(wildcard *.cpp) $(wildcard Midi/*.cpp)
DEPS = $(patsubst %.cpp,%.d,$(SRC))
OBJ = $(SRC:%.cpp=$(BUILDDIR)/%.o)

.PHONY: all clean a.out

all: directories a.out

a.out: $(OBJ)
	$(CXX) $(WARNING) -o $@ $^ $(CXXFLAGS)

-include $(DEPS)

$(BUILDDIR)/%.o: ./%.cpp
	$(CXX) $(WARNING) $(CXXFLAGS) -c $< -o $@ -MMD -MP

directories: ${OUT_DIR}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

clean:
	rm -rf build
	rm -f a.out