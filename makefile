# ----------------------------
# Makefile Options
# ----------------------------

NAME = MSWPR
ICON = icon.png
DESCRIPTION = "It's Minesweeper"
COMPRESSED = YES
COMPRESSED_MODE = zx0
OUTPUT_MAP = NO
ARCHIVED = YES
HAS_PRINTF = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

GFX_FILES = $(wildcard src/gfx/*.c) $(wildcard src/gfx/*.h) src/gfx/convimg.yaml.lst

.PHONY: cleangfx
cleangfx:
	@rm -f $(GFX_FILES)
	@echo Removed built GFX sources

SRC_FILES = $(wildcard src/*.cpp) $(wildcard src/include/*.hpp)

.PHONY: dontuse
dontuse: SRC_FILES
	@echo "this target is just to make CLion happy"
