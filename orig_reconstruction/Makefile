CC      = xc8-cc
MCU     = pic16lf1938

SRCDIR   = .
BUILDDIR = build
DISTDIR  = dist
DFPDIR   = ../dfp
TARGET   = $(DISTDIR)/bekantfirmware

DFP_NAME    = Microchip.PIC12-16F1xxx_DFP
DFP_VERSION = 1.9.258
DFP_ATPACK  = $(DFPDIR)/$(DFP_NAME).$(DFP_VERSION).atpack
DFP_XC8_DIR = $(DFPDIR)/pic12-16f1xxx/xc8

SRCS = \
    main.c \
    system.c \
    user.c \
    configuration_bits.c \
    interrupts.c \
    bekant/bctrl.c \
    bekant/bui.c \
    bekant/bscan.c \
    bekant/oem_tables.c \
    bekant/bcmd_table.c \
    bekant/eeprom_defaults.c \
    bekant/endstop.c \
    btn/btn.c \
    lin/lin_d.c \
    test_sfr_stubs.c

OBJS = $(patsubst %.c,$(BUILDDIR)/%.p1,$(SRCS))

CFLAGS  = -mcpu=$(MCU) -mdfp=$(DFP_XC8_DIR) -std=c99 -O0 -I$(SRCDIR) -DBEKANT_TEST_SYNTAX=1
LDFLAGS = -mcpu=$(MCU) -mdfp=$(DFP_XC8_DIR) -std=c99 -mstack=compiled

.PHONY: all build clean clobber dfp syntax_check

all: build

build: $(DFP_XC8_DIR) $(TARGET).elf

$(TARGET).elf: $(OBJS)
	@mkdir -p $(DISTDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.p1: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Use gcc for a quick syntax check of the C code without XC8.
# This requires defining the test_sfr_stubs. Note that this is
# a partial check; it does not validate the generated PIC assembly.
syntax_check:
	for f in $(SRCS); do \
		gcc -fsyntax-only -I. -DBEKANT_TEST_SYNTAX=1 \
			-include test_sfr_stubs.h $$f || exit 1; \
	done
	@echo "All files passed syntax check."

dfp: $(DFP_XC8_DIR)

$(DFP_XC8_DIR): $(DFP_ATPACK)
	@mkdir -p $(DFPDIR)
	unzip -q -o $(DFP_ATPACK) -d $(DFPDIR)/pic12-16f1xxx

clean:
	rm -rf $(BUILDDIR)/* $(DISTDIR)/*

clobber: clean
