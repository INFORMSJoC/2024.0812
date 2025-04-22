CXX ?= g++

BUILD_DIR ?= build
SRC_DIR ?= src

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
BIN_NAMES := $(patsubst $(SRC_DIR)/%.cpp,%,$(SRCS)) # Extract base names for binaries

CPPFLAGS ?= $(INC_FLAGS) -O2
LDFLAGS ?=

all: $(BIN_NAMES)

# Rule to build each source file as a separate binary
%: $(SRC_DIR)/%.cpp
	@echo "Building: $@"
	@$(MKDIR_P) $(BUILD_DIR)
	@$(CXX) $(CPPFLAGS) $(LDFLAGS) $< -o $(BUILD_DIR)/$@

.PHONY: clean
clean:
	@echo "Deleting build directory"
	@$(RM) -r $(BUILD_DIR)

MKDIR_P ?= mkdir -p
RM ?= rm -f
