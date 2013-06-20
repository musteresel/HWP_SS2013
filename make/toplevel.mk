# Actual toplevel makefile code
# * Grabs general options
# * Includes util makefiles
# * Handles all target
# * Place to include submakefiles

all: compileTest mtask
	@echo "Built $^"

include make/general.mk

#include test/test.mk
include compileTest/compileTest.mk
include mtask/mtask.mk

include make/rules.mk
