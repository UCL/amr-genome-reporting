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
	-I$(UNITY_DIR)/src \
	-I$(CJSON_DIR)

# ====================
# Test executables
# ====================
TESTS := \
	test_unity_sanity

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
	$(OBJ_DIR)/lib/cjson/cJSON.o \
	$(OBJ_DIR)/lib/unity/src/unity.o \
	-o $(TEST_DIR)/$@

# ====================
# Test executables
# ====================
run: all
	for t in $(TESTS); do \
		echo "Running $$t; \
		$(TEST_DIR)/$$t || exit 1; \
	done

# ====================
# Clean
# ====================
clean:
	$(RM) -r $(TEST_DIR)
