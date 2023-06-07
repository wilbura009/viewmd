# == Define the target executable ===
TARGET = viewmd
# ===================================

# ----- Compiler
CC = gcc

PKGCONFIG = $(shell which pkg-config)

CFLAGS = $(shell $(PKGCONFIG) --cflags gtk+-3.0 webkit2gtk-4.1)
LIBS = $(shell $(PKGCONFIG) --libs gtk+-3.0 webkit2gtk-4.1)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

# Define the source and header directories
SRC_DIR = src
BUILT_SRC = $(SRC_DIR)/resources.c
UI_DIR = $(SRC_DIR)/ui

# Get a list of all the source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Get a list of all the header files
HEADERS = $(wildcard $(SRC_DIR)/*.h)

# Define the object files to be generated from the source files
OBJECTS = $(SOURCES:.c=.o) $(BUILT_SRC:.c=.o)

# Generated by bear
COMPILE_CMDS_JSON = compile_commands.json

# UI builder files
G_RSC = $(SRC_DIR)/$(TARGET).gresource.xml
UI = $(wildcard $(UI_DIR)/*.ui)

# ========== Install Variables ===================
SRC_CSS = $(SRC_DIR)/css/webkit.css
VIEWMD_CONFIG_DIR = $(HOME)/.config/viewmd/css
HOME_BIN_DIR = $(HOME)/.bin
USR_LOCAL_BIN_DIR = /usr/local/bin
# ================================================

.PHONY: all clean cleanall build bear run install install-noroot install-dependencies debug-make

# Define the default target
all: $(TARGET)

quiet-all:
	@echo "[2: BUILDING $(TARGET)] ==========================="
	@$(MAKE) -s all > /dev/null 2>&1 && echo "  - [OK]: $(TARGET)" || echo "  - [FAILED]: $(TARGET)"

# TODO: Git clone in opt instead
install: install-dependencies quiet-all
	@echo "[2: INSTALLING $(TARGET)] ========================="
	@mkdir -p $(VIEWMD_CONFIG_DIR) > /dev/null 2>&1 && echo "  - [OK]: mkdir -p $(VIEWMD_CONFIG_DIR)" || echo "  - [FAILED]: mkdir -p $(VIEWMD_CONFIG_DIR)"
	@cp $(SRC_CSS) $(VIEWMD_CONFIG_DIR) && echo "  - [OK]: cp $(SRC_CSS) -> $(VIEWMD_CONFIG_DIR)" || echo "  - [FAILED]: cp $(CSS) -> $(VIEWMD_CONFIG_DIR)"
	@sudo cp $(TARGET) $(USR_LOCAL_BIN_DIR) && echo "  - [OK]: cp $(TARGET) -> $(USR_LOCAL_BIN_DIR)/$(TARGET)" || echo "  - [FAILED]: cp $(TARGET) -> $(USR_LOCAL_BIN_DIR)/$(TARGET)"

install-noroot: install-dependencies quiet-all
	@echo "[2: INSTALLING $(TARGET)] ========================="
	@mkdir -p $(VIEWMD_CONFIG_DIR) > /dev/null 2>&1 && echo "  - [OK]: mkdir -p $(VIEWMD_CONFIG_DIR)" || echo "  - [FAILED]: mkdir -p $(VIEWMD_CONFIG_DIR)"
	@cp $(SRC_CSS) $(VIEWMD_CONFIG_DIR) && echo "  - [OK]: cp $(SRC_CSS) -> $(VIEWMD_CONFIG_DIR)" || echo "  - [FAILED]: cp $(CSS) -> $(VIEWMD_CONFIG_DIR)"
	@mkdir -p $(HOME)/.bin > /dev/null 2>&1 && echo "  - [OK]: mkdir -p $(HOME_BIN_DIR)" || echo "  - [FAILED]: mkdir -p $(HOME_BIN_DIR)"
	@cp $(TARGET) $(HOME)/.bin && echo "  - [OK]: cp $(TARGET) -> $(HOME_BIN_DIR)" || echo "  - [FAILED]: cp $(TARGET) -> $(HOME_BIN_DIR)"

install-dependencies:
	@echo "[1: INSTALLING DEPENDENCIES] ======================"
	@sudo apt update > /dev/null 2>&1 && echo "  - [OK]: apt update" || echo "  - [FAILED]: apt update"
	@sudo apt install -y pkg-config > /dev/null 2>&1 && echo "  - [OK]: pkg-config " || echo "  - [FAILED] pkg-config"
	@sudo apt install -y libgtk-3-dev > /dev/null 2>&1 && echo "  - [OK]: libgtk-3-dev " || echo "  - [FAILED] libgtk-3-dev"
	@sudo apt install -y libwebkit2gtk-4.1-dev > /dev/null 2>&1 && echo "  - [OK]: libwebkit2gtk-4.1-dev " || echo "  - [FAILED] libwebkit2gtk-4.1-dev"
	@sudo apt install -y > /dev/null 2>&1 && echo "  - [OK]: gcc " || echo "  - [FAILED] gcc"

$(BUILT_SRC): $(G_RSC) $(UI)
	$(GLIB_COMPILE_RESOURCES) $(G_RSC) --target=$@ --sourcedir=$(UI_DIR) --generate-source

# Define a rule to compile the source files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Define the target executable and link the object files
$(TARGET): $(OBJECTS)
	$(CC) -o $(@F) $(OBJECTS) $(LIBS)

# Define a target to run the executable
run: bear
	./$(TARGET)

# Creates a compile_commands.json file for use with clangd
bear:
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
	@echo "SRC_CSS: $(SRC_CSS)"
	@echo "VIEWMD_CONFIG_DIR: $(VIEWMD_CONFIG_DIR)"


# VPATH:
# ------
# With VPATH set to VPATH = src:../header
# foo.o : foo.c
# is interpreted as if it were written like this:
# foo.o : src/foo.c
#
# Automatic Variables:
# --------------------
# $^ : Get list of all prerequisites
# $< : Get first prerequisite
# $@ : Get target value
#
# gcc `pkg-config --cflags gtk+-3.0` -o main main.c `pkg-config --libs gtk+-3.0`
# glib-compile-resources .gresource.xml --target=source/resources.c --generate-source