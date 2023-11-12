# ----------------------------
# Makefile Options
# ----------------------------

NAME = MSWPR
ICON = icon.png
DESCRIPTION = "It's minesweeper"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
