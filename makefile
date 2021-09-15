NAME        ?= SPODGE84
DESCRIPTION ?= "Avoid the flying objects!"
ICON        ?= icon.png
COMPRESSED  ?= YES
ARCHIVED    ?= YES

CFLAGS      ?= -Wall -Wextra -Oz
CXXFLAGS    ?= -Wall -Wextra -Oz

ifndef CEDEV
$(error CEDEV enviroment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk
