# Executables: quicksort.exe, tbbQuicksort.exe
TARGETS = quicksort tbbQuicksort

# Default to 64 bits, override with e.g. make BITS=32
BITS ?= 64

ODIR     := ./obj$(BITS)
TMPDIR   := $(abspath $(ODIR)/tmp)
__odir   := $(shell mkdir -p $(ODIR) $(TMPDIR))
export TMPDIR

CXX       = g++
LD        = g++
CXXFLAGS += -MMD -O3 -m$(BITS) -g -std=c++17 -fPIC -march=native -Wall -Werror -DNDEBUG
LDFLAGS  += -m$(BITS) -lpthread

EXEFILES = $(patsubst %, $(ODIR)/%.exe, $(TARGETS))
DFILES   = $(patsubst %, $(ODIR)/%.d, $(TARGETS))

-include $(DFILES)

.DEFAULT_GOAL = all
.PHONY: all clean
.PRECIOUS: $(EXEFILES)
all: $(EXEFILES)

$(ODIR)/%.exe: %.cc
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)

$(ODIR)/tbbQuicksort.exe: tbbQuicksort.cpp
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS) -ltbb

clean:
	@echo Cleaning up...
	@rm -rf $(ODIR)
