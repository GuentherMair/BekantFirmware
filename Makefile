CC      = xc8-cc
MCU     = pic16lf1938

SRCDIR   = src
BUILDDIR = build
DISTDIR  = dist
DFPDIR   = dfp
TARGET   = $(DISTDIR)/bekantfirmware

# Device Family Pack — download once with: make dfp
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
    btn/btn.c \
    lin/lin_d.c

OBJS = $(patsubst %.c,$(BUILDDIR)/%.p1,$(SRCS))

CFLAGS  = -mcpu=$(MCU) -mdfp=$(DFP_XC8_DIR) -std=c99 -O0 -I$(SRCDIR)
LDFLAGS = -mcpu=$(MCU) -mdfp=$(DFP_XC8_DIR) -std=c99 -mstack=compiled

.PHONY: all build clean clobber dfp

all: build

build: $(DFP_XC8_DIR) $(TARGET).elf

$(TARGET).elf: $(OBJS)
	@mkdir -p $(DISTDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.p1: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Download and extract the Device Family Pack
dfp: $(DFP_XC8_DIR)

$(DFP_XC8_DIR): $(DFP_ATPACK)
	@mkdir -p $(DFPDIR)
	unzip -q -o $(DFP_ATPACK) -d $(DFPDIR)/pic12-16f1xxx

$(DFP_ATPACK):
	curl -fL -o $(DFP_ATPACK) \
	    "https://packs.download.microchip.com/$(DFP_NAME).$(DFP_VERSION).atpack"

clean:
	rm -rf $(BUILDDIR)/* $(DISTDIR)/* $(DFPDIR)/*

clobber: clean
