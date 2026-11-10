PROJECT = blink
BUILD_DIR = build

SHARED_DIR = ../my-common-code
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
CFILES := $(call rwildcard,./,*.c)
$(info CFILES = [$(CFILES)])

DEVICE=stm32f411re

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
