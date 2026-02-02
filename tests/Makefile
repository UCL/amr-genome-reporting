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

CC ?= cc
RM ?= rm -f

CFLAGS := -std=c18 -Wall -Wextra -g

# ====================
# Paths
# ====================
ROOT_DIR := ..
OBJ_DIR  := $(ROOT_DIR)/build/obj
TEST_DIR := $(ROOT_DIR)/build/test

UNITY_DIR := $(ROOT_DIR)/lib/unity
CJSON_DIR := $(ROOT_DIR)/lib/cjson

# ====================
# Includes
# ====================
INCLUDES := \
	-I$(ROOT_DIR)/include \
	-I$(ROOT_DIR)/src \
	-I$(ROOT_DIR)/src/io \
	-I$(UNITY_DIR)/src \
	-I$(CJSON_DIR)

# ====================
# Test executables
# ====================
TESTS := \
	test_unity_sanity \
	test_read_text

.PHONY: all clean run

all: $(TESTS)

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

# ====================
# Build rules
# ====================
test_%: test_%.c | $(TEST_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) \
	$< \
	$(OBJ_DIR)/src/io/read_text.o \
	$(OBJ_DIR)/lib/cjson/cJSON.o \
	$(OBJ_DIR)/lib/unity/src/unity.o \
	-o $(TEST_DIR)/$@

# ====================
# Test executables
# ====================
run: all
	for t in $(TESTS); do \
		echo "Running $$t"; \
		$(TEST_DIR)/$$t || exit 1; \
	done

# ====================
# Clean
# ====================
clean:
	$(RM) -r $(TEST_DIR)
