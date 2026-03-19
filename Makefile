# Made with Claude
# --- Paths -------------------------------------------------------------------
SRC_DIR := src
OUT_DIR := out
TARGET  := app

SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(SRCS))
BIN     := $(OUT_DIR)/$(TARGET)

# --- Configuration -----------------------------------------------------------
CC      := gcc
OPT     := -O0 -g3
WARNS   := -Wall -Wextra -fsanitize=address,leak,undefined \
           -Wshadow -Wundef -Wcast-align -Wfloat-equal
CFLAGS  := $(WARNS) $(OPT) -std=c23 -Wpedantic
LDFLAGS := -I$(SRC_DIR)

RUNFLAGS := 

# --- Rules -------------------------------------------------------------------
.PHONY: all run clean

all: $(BIN)
	@echo "Built $(BIN)"

# Link
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Compile - create out/ automatically if it doesn't exist
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create out/ directory
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)

run: all
	@echo "Running $(BIN) $(RUNFLAGS)"
	@echo ""

	@$(BIN) $(RUNFLAGS)
