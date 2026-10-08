PROJECT_ROOT := $(abspath ../../..)
SHORT_NAME := RRJ
FULL_NAME := Road Rash: Jailbreak
CONFIG ?= Release
APP_SOURCES := $(wildcard $(PROJECT_ROOT)/src/*.c)
APP_DEFINES := -DWND_TITLE='"Road Rash: Jailbreak (xport)"' -DWND_WIDTH=960 -DWND_HEIGHT=768 -DFIELD_RATE=60

include ../../../../xport/src/platform/web/Makefile

WEB_PACKAGER := $(PROJECT_ROOT)/tools/rrj_web_packager.py
