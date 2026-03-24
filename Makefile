# Made with the help of AI
# --- Paths -------------------------------------------------------------------
SRC_DIR := src
CAKE_DIR := cake_out
OUT_DIR := out
TARGET  := app

SRCS    := $(wildcard $(SRC_DIR)/*.c)
# .i = file extension for C source code which should not be preprocessed.
CAKE_IRS := $(patsubst $(SRC_DIR)/%.c, $(CAKE_DIR)/%.i, $(SRCS))
OBJS    := $(patsubst $(CAKE_DIR)/%.i, $(OUT_DIR)/%.o, $(CAKE_IRS))
BIN     := $(OUT_DIR)/$(TARGET)

# --- Configuration -----------------------------------------------------------
CC      := gcc
CAKE_EXEC   := ./cake
OPT     := -O0 -g3
WARNS   := -Wall -Wextra -fsanitize=address,leak,undefined \
           -Wshadow -Wundef -Wcast-align -Wfloat-equal
CFLAGS  := $(WARNS) $(OPT) -std=c89 -Wpedantic
LDFLAGS := -I$(CAKE_DIR)
CAKE_FLAGS := -I$(SRC_DIR)

RUNFLAGS := 

# --- Rules -------------------------------------------------------------------
# Targets that are not files, but rather names for a recipe
.PHONY: all run clean clean_build clean_run b r echo

# Files that shall not be deleted upon error (I think?)
.PRECIOUS: $(CAKE_DIR)/%.i

# Just in case, but the first non-dot target is the default either way
.DEFAULT_GOAL := all

all: $(BIN)
	@echo "Built $(BIN)"

# Link
# $^ = The names of all the prerequisites, with spaces between them.
# $@ = The file name of the target of the rule.
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Compile
# | = separates normal prerequisites (left) and order-only prerequisites (right).
#     The latter doesn't require remaking the target when modified.
# % = wildcard, matches any number of characters
# $< = The name of the first prerequisite.
# gcc -c = Compile or assemble the source files, but do not link.
$(OUT_DIR)/%.o: $(CAKE_DIR)/%.i | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create out/ directory
# mkdir -p = no error if existing, make parent directories as needed
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Preprocess
$(CAKE_DIR)/%.i: $(SRC_DIR)/%.c | $(CAKE_DIR)
	$(CAKE_EXEC) $(CAKE_FLAGS) $< -o $@

$(CAKE_DIR):
	mkdir -p $(CAKE_DIR)

clean:
	rm -rf $(OUT_DIR) $(CAKE_DIR)

run: all
	@echo "Running $(BIN) $(RUNFLAGS)"
	@echo ""

	@$(BIN) $(RUNFLAGS)

clean_build: clean all
clean_run: clean run

b: all
r: run
