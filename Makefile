# Made with the help of AI
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
LIBS    := -lncurses -lpanel
CFLAGS  := $(WARNS) $(OPT) -std=c23 -Wpedantic
LDFLAGS := -I$(SRC_DIR) $(LIBS)

RUNFLAGS := 

# --- Rules -------------------------------------------------------------------
# Targets that are not files, but rather names for a recipe
.PHONY: all check run clean clean_build clean_run clean_check b c r

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
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create out/ directory
# mkdir -p = no error if existing, make parent directories as needed
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR)

check:
	@echo "Checking without codegen..."
	$(CC) $(CFLAGS) -fsyntax-only $(SRCS)
	@echo "Source checked successfully"

run: all
	@echo "Running $(BIN) $(RUNFLAGS)"
	@echo ""

	@$(BIN) $(RUNFLAGS)

clean_build: clean all
clean_check: clean check
clean_run: clean run

b: all
c: check
r: run
