LLVM_CONFIG = llvm-config-18
CXX         = g++
CXXFLAGS    = -g -O2 -Wall -Iinclude -Itabulate/include \
			  $(shell $(LLVM_CONFIG) --cxxflags) -std=c++17

LDFLAGS     = $(shell $(LLVM_CONFIG) --ldflags)
LDLIBS      = -Wl,--start-group $(shell $(LLVM_CONFIG) --libs core analysis passes) -Wl,--end-group \
              $(shell $(LLVM_CONFIG) --system-libs)

SRC_DIR  = src
OBJ_DIR  = build
OUT      = quark

SRC = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/passes/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(OUT)

clang: CXX = clang++
clang: $(OUT)

$(OUT): $(OBJ)
	@echo "Linking..."
	@mkdir -p out
	@$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) *.o *.ll
	@mkdir build build/passes

format:
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format -style=file -i {} \;
