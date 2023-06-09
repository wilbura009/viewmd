# == Define the target executable ===
TARGET = viewmd
# ===================================

# Compiler
CC = gcc

PKGCONFIG = $(shell which pkg-config)

CFLAGS = $(shell $(PKGCONFIG) --cflags gtk+-3.0 webkit2gtk-4.1)
LIBS = $(shell $(PKGCONFIG) --libs gtk+-3.0 webkit2gtk-4.1)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

# Define the source and header directories
SRC_DIR = src
BUILT_SRC = $(SRC_DIR)/resources.c
DATA_DIR = data

# Get a list of all the source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Get a list of all the header files
HEADERS = $(wildcard $(SRC_DIR)/*.h)

# Define the object files to be generated from the source files
OBJECTS = $(SOURCES:.c=.o) $(BUILT_SRC:.c=.o)

# Generated by bear
COMPILE_CMDS_JSON = compile_commands.json

# Resource files
G_RSC = $(SRC_DIR)/$(TARGET).gresource.xml
UI = $(wildcard $(SRC_DIR)/*.ui)
DATA = $(wildcard $(DATA_DIR/*))

# Installation directories
HOME_BIN_DIR = $(HOME)/.bin
USR_LOCAL_BIN_DIR = /usr/local/bin

# ======== Test Variables ========================
TEST_INPUT = tests/input/ipsum.md
# ================================================

.PHONY: all bear run \
	install install-home install-dependencies \
	uninstall uninstall-home uninstall-all \
	clean cleanall \
  debug-make 

# Define the default target
all: $(TARGET)

quiet-all:
	@echo "[3: BUILDING $(TARGET)] ============================================="
	@$(MAKE) -s cleanall
	@$(MAKE) -s all > /dev/null 2>&1 && \
		echo "  - [OK]: $(TARGET)" || \
		echo "  - [FAILED]: $(TARGET)"
	@echo

install: install-dependencies quiet-all
	@echo "[4: INSTALLING $(TARGET)] ==========================================="
	@sudo cp $(TARGET) $(USR_LOCAL_BIN_DIR) && \
		echo "  - [OK]: cp $(TARGET) -> $(USR_LOCAL_BIN_DIR)/$(TARGET)" || \
		echo "  - [FAILED]: cp $(TARGET) -> $(USR_LOCAL_BIN_DIR)/$(TARGET)"
	@echo

install-home: install-dependencies quiet-all
	@echo "[4: INSTALLING $(TARGET)] ==========================================="
	@mkdir -p $(HOME)/.bin > /dev/null 2>&1 && \
		echo "  - [OK]: mkdir -p $(HOME_BIN_DIR)" || \
		echo "  - [FAILED]: mkdir -p $(HOME_BIN_DIR)"
	@cp $(TARGET) $(HOME)/.bin && \
		echo "  - [OK]: cp $(TARGET) -> $(HOME_BIN_DIR)" || \
		echo "  - [FAILED]: cp $(TARGET) -> $(HOME_BIN_DIR)"
	@echo

apt-update:
	@echo "[1: UPDATING APT] ==================================================="
	@sudo apt update > /dev/null 2>&1 && \
		echo "  - [OK]: apt update" || \
		echo "  - [FAILED]: apt update"
	@echo

install-dependencies: apt-update
	@echo "[2: INSTALLING DEPENDENCIES] ========================================"
	@sudo apt install -y gcc > /dev/null 2>&1 && \
		echo "  - [OK]: gcc " || \
		echo "  - [FAILED] gcc"
	@sudo apt install -y pkg-config > /dev/null 2>&1 && \
		echo "  - [OK]: pkg-config " || \
		echo "  - [FAILED] pkg-config"
	@sudo apt install -y libgtk-3-dev > /dev/null 2>&1 && \
		echo "  - [OK]: libgtk-3-dev " || \
		echo "  - [FAILED] libgtk-3-dev"
	@sudo apt install -y libwebkit2gtk-4.1-dev > /dev/null 2>&1 && \
		echo "  - [OK]: libwebkit2gtk-4.1-dev " || \
		echo "  - [FAILED] libwebkit2gtk-4.1-dev"
	@sudo apt install -y pandoc > /dev/null 2>&1 && \
		echo "  - [OK]: pandoc " || \
		echo "  - [FAILED] pandoc"
	@echo

uninstall-all: uninstall uninstall-home

uninstall:
	@echo "[UNINSTALLING $(TARGET)] ============================================"
	@sudo rm $(USR_LOCAL_BIN_DIR)/$(TARGET) > /dev/null 2>&1 && \
		echo "  - [OK]: rm $(USR_LOCAL_BIN_DIR)/$(TARGET)" || \
		echo "  - [FAILED]: rm $(USR_LOCAL_BIN_DIR)/$(TARGET)"
	@echo

uninstall-home:
	@echo "[UNINSTALLING $(TARGET)] ============================================"
	@sudo rm $(HOME_BIN_DIR)/$(TARGET) > /dev/null 2>&1 && \
		echo "  - [OK]: rm $(HOME_BIN_DIR)/$(TARGET)" || \
		echo "  - [FAILED]: rm $(HOME_BIN_DIR)/$(TARGET)"
	@echo

$(BUILT_SRC): $(G_RSC) $(UI) $(DATA_DIR)
	$(GLIB_COMPILE_RESOURCES) $(G_RSC) --target=$@ --sourcedir=$(SRC_DIR) --generate-source

# Define a rule to compile the source files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Define the target executable and link the object files
$(TARGET): $(OBJECTS)
	$(CC) -o $(@F) $(OBJECTS) $(LIBS)

# Define a target to run the executable with the test input
run: bear
	./$(TARGET) $(TEST_INPUT)

# Creates a compile_commands.json file for use with clangd
bear:
	@$(MAKE) -s clean
	@bear -- make -s > /dev/null || echo "bear FAILED: Is it installed?"
	@$(MAKE) -s clean

# Define a target to clean up generated files
clean:
	rm -f $(OBJECTS)
	rm -f $(BUILT_SRC)

# Define a target to clean up generated files
cleanall:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	rm -f $(BUILT_SRC)
	rm -f $(COMPILE_CMDS_JSON)

debug-make:
	@echo "SOURCES: $(SOURCES)"
	@echo "HEADERS: $(HEADERS)"
	@echo "OBJECTS: $(OBJECTS)"
	@echo "BUILT_SRC: $(BUILT_SRC)"
	@echo "G_RSC: $(G_RSC)"
	@echo "UI: $(UI)"
	@echo "LIBS: $(LIBS)"
	@echo "GLIB_COMPILE_RESOURCES: $(GLIB_COMPILE_RESOURCES)"
	@echo "COMPILE_CMDS_JSON: $(COMPILE_CMDS_JSON)"

