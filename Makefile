CXX := clang++

TARGET := phosphor
BUILD := ./build

APP := ./phosphor
SRC := $(APP)/src
VEND := $(APP)/vendor/
INCL := $(APP)/include/
LIB_DIR := $(APP)/libs/

SRCS := $(wildcard $(SRC)/*.cpp)

CFLAGS := -Wall -Wextra -pedantic
LDFLAGS := -L$(LIB_DIR) -lraylib -L/opt/homebrew/opt/llvm/lib/c++ -lc++abi -I/opt/homebrew/opt/llvm/include/c++/v1
LUA := -I/opt/homebrew/include/lua -L/opt/homebrew/lib -llua -lm
FRAMEWORKS := -framework CoreFoundation -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL

$(BUILD)/$(TARGET):
	@mkdir -p $(@D)
	$(CXX) -I$(INCL) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS) $(FRAMEWORKS) $(LUA)