# count_frequency.exe from count_frequency.cc
TARGETS = count_frequency

BITS ?= 64
ODIR   := ./obj$(BITS)
__odir := $(shell mkdir -p $(ODIR))

CXX      = g++
CXXFLAGS = -MMD -O3 -m$(BITS) -g -std=c++17 -Wall -Werror -DNDEBUG
LDFLAGS  = -m$(BITS) -lpthread

EXEFILES = $(patsubst %, $(ODIR)/%.exe, $(TARGETS))
DFILES   = $(patsubst %, $(ODIR)/%.d, $(TARGETS))
-include $(DFILES)

.PHONY: all clean
all: $(EXEFILES)

$(ODIR)/%.exe: %.cc
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)

clean:
	@rm -rf $(ODIR)
