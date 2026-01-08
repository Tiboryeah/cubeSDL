CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -Iext/SDL2/include/SDL2 -Dmain=SDL_main
LDFLAGS = -Lext/SDL2/lib -lmingw32 -lSDL2main -lSDL2 
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = $(BIN_DIR)/3d_view_cpp.exe

all: directories $(TARGET)

directories:
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	@if exist "$(BIN_DIR)" rmdir /s /q "$(BIN_DIR)"
	@echo Limpieza completada.

run: all
	$(TARGET)

.PHONY: all clean run
