# Executables to build.  We assume each .exe is built from just one .cc file.
TARGETS = mm

# Default to 64 bits, but allow overriding on command line
BITS     ?= 64

# Give name to output folder, and ensure it is created before any compilation
ODIR     := ./obj$(BITS)
__odir   := $(shell mkdir -p $(ODIR))

# Basic tool configuration for gcc/g++
CXX       = g++
LD        = g++
CXXFLAGS += -MMD -O3 -m$(BITS) -g -std=c++20 -g -fPIC -march=native -Wall -Werror -Wno-vla -DNDEBUG
LDFLAGS  += -m$(BITS) -lpthread

# Names of all .exe files, .o files, and .d files
EXEFILES  = $(patsubst %, $(ODIR)/%.exe, $(TARGETS))
OFILES    = $(patsubst %, $(ODIR)/%.o, $(TARGETS))
DFILES    = $(patsubst %, $(ODIR)/%.d, $(TARGETS))

# dependencies for the .o files built from .cc files in this folder
-include $(DFILES)

# The default target builds all executables in a two step (compile, link)
# process
.DEFAULT_GOAL = all
.PHONY: all clean
.PRECIOUS: $(OFILES) $(EXEFILES)
all: $(EXEFILES)

# Build a .exe file from a .cc file
$(ODIR)/%.exe: %.cc
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)

# Link a .o file into a .exe
$(ODIR)/%.exe: $(ODIR)/%.o
	@echo "[LD] $^ --> $@"
	@$(CXX) $^ -o $@ $(LDFLAGS)

# clean by clobbering the build folder
clean:
	@echo Cleaning up...
	@rm -rf $(ODIR)
