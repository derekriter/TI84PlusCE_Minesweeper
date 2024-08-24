# ----------------------------
# Makefile Options
# ----------------------------

NAME = MSWPR
ICON = icon.png
DESCRIPTION = "It's minesweeper"
COMPRESSED = YES
COMPRESSED_MODE = zx0
OUTPUT_MAP = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

GFX_FILES = $(wildcard src/gfx/*.c) $(wildcard src/gfx/*.h) src/gfx/convimg.yaml.lst

.PHONY: cleangfx
cleangfx: $(GFX_FILES)
	@rm $(GFX_FILES)
	@echo Removed built GFX sources
