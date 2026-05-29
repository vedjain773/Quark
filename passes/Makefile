LLVM_CONFIG = llvm-config-18
CXX = clang++-18

CXXFLAGS = `$(LLVM_CONFIG) --cxxflags` \
           -fPIC \
           -shared \
           -fno-rtti \
           -std=c++17

LDFLAGS = `$(LLVM_CONFIG) --ldflags --system-libs --libs core passes support`

TARGET = libMyPass.so

all:
	@echo Building...
	@$(CXX) $(CXXFLAGS) PowerTransform.cpp -o $(TARGET) $(LDFLAGS)

run:
	@echo Running Passes...
	@opt-18 -load-pass-plugin=./libMyPass.so -passes="power-transform" output.ll -disable-output

clean:
	rm -f $(TARGET) test.ll
