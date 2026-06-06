CC      = xc8-cc
MCU     = pic16lf1938

BUILDDIR = build/default
DISTDIR  = dist/default
TARGET   = $(DISTDIR)/bekantfirmware

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

CFLAGS  = -mcpu=$(MCU) -std=c99 -O0 -I.
LDFLAGS = -mcpu=$(MCU) -std=c99 -mstack=compiled

.PHONY: all build clean clobber

all: build

build: $(TARGET).elf

$(TARGET).elf: $(OBJS)
	@mkdir -p $(DISTDIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.p1: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR) $(DISTDIR)

clobber: clean
