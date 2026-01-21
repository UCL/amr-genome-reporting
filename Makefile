# ====================
# Toolchain
# ====================
CC	?= cc
AR	?= ar
RM	?= rm -f

CSTD	:= -std=c18
WARN	:= -Wall -Wextra -Wpedantic
OPT	?= -O2
DEBUG	?= -g

CFLAGS	:= $(CSTD) $(WARN) $(OPT) $(DEBUG)
LDFLAGS	:=

# ====================
# Project
# ====================
PROJECT	:= amr_gen_reporting

INCLUDES := \
	-Iinclude \
	-Isrc \
	-Ilib/cjson \
	-Ilib/pdfio

SRC_DIRS := \
	src/core \
	src/io \
	src/pdf \
	src

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
BIN_DIR   := $(BUILD_DIR)/bin

# ====================
# Sources
# ====================
APP_SRC := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
APP_OBJ := $(patsubst %.c,$(OBJ_DIR)/%.o,$(APP_SRC))

LIB_SRC := \
	lib/cjson/cJSON.c

LIB_OBJ := \
	$(patsubst %.c,$(OBJ_DIR)/%.o,$(LIB_SRC))

# Unity (test-only)
UNITY_SRC := lib/unity/src/unity.c
UNITY_OBJ := $(OBJ_DIR)/lib/unity/src/unity.o

# ====================
# Targets
# ====================
.PHONY: all clean test dirs

all: dirs $(BIN_DIR)/$(PROJECT)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR) $(BUILD_DIR)/test

# ====================
# Build rules
# ====================
$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Production binary (no Unity)
$(BIN_DIR)/$(PROJECT): $(APP_OBJ) $(LIB_OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

# ====================
# Tests
# ====================
test: $(UNITY_OBJ)
	$(MAKE) -C tests

# ====================
# Clean
# ====================
clean:
	$(RM) -r $(BUILD_DIR)
