# Actual toplevel makefile code
# * Grabs general options
# * Includes util makefiles
# * Handles all target
# * Place to include submakefiles

all:  robot
	@echo "Built $^"

include make/general.mk
include robot.mk

include make/rules.mk
