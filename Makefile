LLVM_CONFIG = /home/ved/Documents/minic/llvm-18/bin/llvm-config

CXX      = g++

CXXFLAGS = -g -O0 -Wall -Iinclude \
           $(shell $(LLVM_CONFIG) --cxxflags) \
           -fno-rtti -D_GLIBCXX_USE_CXX11_ABI=1

LDFLAGS  = $(shell $(LLVM_CONFIG) --ldflags)
LDLIBS   = $(shell $(LLVM_CONFIG) --libs all) $(shell $(LLVM_CONFIG) --system-libs)

SRC_DIR  = src
OBJ_DIR  = build
OUT      = out/main

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
