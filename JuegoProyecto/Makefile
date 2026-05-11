# ========================================
#  Makefile (Ubuntu + SFML + Renderizador)
# ========================================

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# SFML (Ubuntu/WSL)
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))

BIN_DIR := bin
BUILD_DIR := build
TARGET := $(BIN_DIR)/prueba_lector

# ===== Regla principal =====
all: $(TARGET)

# ===== Construcción del ejecutable =====
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

# ===== Regla general para objetos =====
build/%.o: src/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===== Ejecutar =====
run: $(TARGET)
	./$(TARGET)

# ===== Limpiar =====
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) *.o doc

# ===== Forzar recompilación =====
rebuild: clean all

# ===== Documentación =====
doc:
	@mkdir -p doc
	doxygen Doxyfile
	@xdg-open doc/html/index.html || true
