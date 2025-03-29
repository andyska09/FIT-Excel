TARGET = main.out
OBJECT_DIR = bin
SOURCE_DIR = .

CXX=g++
LD=g++
CXXFLAGS=-std=c++20 -Wall -pedantic -Wextra -fsanitize=address -g
LDFLAGS=-fsanitize=address -L./x86_64-linux-gnu -lexpression_parser

HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))
DEPENDENCIES := $(OBJECTS:%.o=%.d)

all: $(TARGET)

$(OBJECT_DIR)/%.o : $(SOURCE_DIR)/%.cpp
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

$(TARGET): $(OBJECTS)
	$(LD) $^ $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJECT_DIR)/
	rm -f $(TARGET)

run : $(TARGET)
	./$(TARGET)

.PHONY: clean all run

# Makes sure we recompile when changing the test flag
$(OBJECT_DIR)/main.o: Makefile

-include $(DEPENDENCIES)
