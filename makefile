# ----------------------------
# Makefile Options
# ----------------------------

NAME = MSWPR2
ICON = icon.png
DESCRIPTION = "It's minesweeper"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
