PROJECT_ROOT := $(abspath ../../..)
SHORT_NAME := RRJ
FULL_NAME := Road Rash: Jailbreak
CONFIG ?= Release
APP_SOURCES := $(wildcard $(PROJECT_ROOT)/src/*.c)
APP_DEFINES := -DWND_TITLE='"Road Rash: Jailbreak (xport)"' -DXPORT_GAME_GPU_GUEST_DISPATCH=1 -DWND_WIDTH=960 -DWND_HEIGHT=720 -DFIELD_RATE=60

ifeq ($(CONFIG),Release)
APP_DEFINES += -DXPORT_DISABLE_FUNCTION_MARKERS
endif

WEB_PACKAGER_FLAGS += --extra-file "$(PROJECT_ROOT)/tools/duckstation/data/user/bios/scph5502.bin" tools/duckstation/data/user/bios/scph5502.bin

include ../../../../xport/src/platform/web/Makefile

# Preserve nested menu and resource calls across browser yields
LINK_FLAGS += -sASYNCIFY_STACK_SIZE=65536

ifeq ($(CONFIG),Release)
# Avoid the Binaryen O2/O3 failure after function pointer emulation
LINK_FLAGS += -O1
endif

WEB_PACKAGER := $(PROJECT_ROOT)/tools/rrj_web_packager.py
