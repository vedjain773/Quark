LLVM_CONFIG = llvm-config-18
CXX         = g++
CXXFLAGS    = -g -O0 -Wall -Iinclude $(shell $(LLVM_CONFIG) --cxxflags)
LDFLAGS     = $(shell $(LLVM_CONFIG) --ldflags)
LDLIBS      = -Wl,--start-group $(shell $(LLVM_CONFIG) --libs all) -Wl,--end-group \
              $(shell $(LLVM_CONFIG) --system-libs)

SRC_DIR  = src
OBJ_DIR  = build
OUT      = minic

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(OUT)

$(OUT): $(OBJ)
	@echo "Linking..."
	@mkdir -p out
	@$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) out
