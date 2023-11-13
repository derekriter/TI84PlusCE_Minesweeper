# ----------------------------
# Makefile Options
# ----------------------------

NAME = MSWPR_B
ICON = icon.png
DESCRIPTION = "It's minesweeper"
COMPRESSED = NO
OUTPUT_MAP = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
