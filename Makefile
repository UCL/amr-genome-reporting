# ====================
# AMR Genome Reporting CLI
# Copyright (C) 2026  University College London.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
# ====================

# ====================
# Toolchain
# ====================
CC	?= cc
AR	?= ar
RM	?= rm -f

# ====================
# Build mode
# ====================
BUILD	?= release # release | debug

BASE_CFLAGS := -std=c18 -Wall -Wextra -Wpedantic
BASE_LDFLAGS :=

RELEASE_CFLAGS := -O2
RELEASE_LDFLAGS :=

DEBUG_CFLAGS := -O0 -g3 -fno-omit-frame-pointer
DEBUG_LDFLAGS :=

SANITISERS := -fsanitize=address

ifeq ($(BUILD),debug)
	CFLAGS := $(BASE_CFLAGS) $(DEBUG_CFLAGS) $(SANITISERS)
	LDFLAGS := $(BASE_LDFLAGS) $(DEBUG_LDFLAGS) $(SANITISERS)
else
	CFLAGS := $(BASE_CFLAGS) $(RELEASE_CFLAGS)
	LDFLAGS := $(BASE_LDFLAGS) $(RELEASE_LDFLAGS)
endif

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
	src/json \
	src/pdf \
	src

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
BIN_DIR   := $(BUILD_DIR)/bin

# ====================
# Documentation
# ====================
DOXYGEN ?= doxygen
DOXYFILE := Doxyfile
DOCS_DIR := $(BUILD_DIR)/docs

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
.PHONY: all clean test docs dirs debug

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
test: $(APP_OBJ) $(LIB_OBJ) $(UNITY_OBJ)
	$(MAKE) -C tests

# ====================
# Documentation
# ====================
docs:
	$(DOXYGEN) $(DOXYFILE)
	@echo "Documentation generated in $(DOCS_DIR)/html/index.html"

# ====================
# Debug
# ====================
debug:
	$(MAKE) BUILD=debug

# ====================
# Clean
# ====================
clean:
	$(RM) -r $(BUILD_DIR)
