# Disassembly comparison: current build vs original OEM

This document explains the size gap between our current build
(`dist/bekantfirmware.hex`, 15,476 bytes) and the original OEM hex
(`oem_reconstruction/disassembly/orginafirm.hex`, 93,697 bytes) and
catalogs what is missing.

## Raw numbers

| Hex file                              | Size (bytes) | Code range      | Words of code |
| ------------------------------------- | ------------ | --------------- | ------------- |
| `dist/bekantfirmware.hex` (current)   | 15,476       | 0x0000 – 0x0F95 | ~2,797        |
| `orginafirm.hex` (original)           | 93,697       | 0x0000 – 0x3BE2 | ~7,200        |
| of which 0x8000–0x8008 (config words) |              |                 | 6             |
| of which 0xF000–0xF0FF (EEPROM)       |              |                 | 256           |

The current build is a fork of ivanwick, which is itself what
we believe to be a simplification of the OEM firmware.
ivanwick's code lives in the range **0x0000–0x07FF** (≈ 2,000
words ≈ 4,000 bytes of flash); with what we believe to be the
recovered endstop detector wired in (via
`src/bekant/endstop.c`) the build extends to **0x0F95** (≈ 2,797
words ≈ 5,594 bytes of flash).

The original OEM firmware appears to fill the full 16 Kword
flash with code in two non-contiguous regions:

- **0x0000–0x07FF** — what looks like the common ivanwick-shaped
  control plane (interrupts, main, BCMD dispatch)
- **0x0800–0x1FFF** — what we believe to be the OEM-specific state
  machine, data tables, endstop detector, button state machine
- **0x3800–0x3BFF** — what we believe to be the interrupt-driven
  scheduler and EEPROM helpers

The middle 0x2000–0x37FF is mostly empty (0x3fff = "unprogrammed"
on PIC16, so it shows as `3fff movwi -.1[1]` in the disassembly).

## What the new build contains

`pic-objdump -d dist/bekantfirmware.elf` shows the new build has
roughly 30 separate `text*` psects, all in the 0x0000–0x0F95
range. The symbol table lists these functions (and their start
addresses):

| Address | Function                             | Source file                |
| ------- | ------------------------------------ | -------------------------- |
| 0x0002  | `start` (reset vector)               | XC8 runtime                |
| 0x0004  | `isr` (interrupt vector)             | `interrupts.c`             |
| 0x0439  | `lin_reset_usart`                    | `lin/lin_d.c`              |
| 0x043E  | `lin_init_hw`                        | `lin/lin_d.c`              |
| 0x0444  | `lin_tx_frame` / `lin_txrx_blocking` | `lin/lin_d.c`              |
| 0x0453  | `bscan_scan_slot_first`              | `bekant/bscan.c`           |
| 0x046F  | `__eetoi` (EEPROM read)              | XC8 runtime                |
| 0x04A2  | `bctrl_init`                         | `bekant/bctrl.c`           |
| 0x04B1  | `bctrl_timer`                        | `bekant/bctrl.c`           |
| 0x04C1  | `bscan_init` / `InitApp`             | `bekant/bscan.c`, `user.c` |
| 0x04D2  | `ConfigureOscillator`                | `system.c`                 |
| 0x04FB  | `InitUsart`                          | `user.c`                   |
| 0x0513  | `btn_init`                           | `btn/btn.c`                |
| 0x052B  | `lin_txrx`                           | `lin/lin_d.c`              |
| 0x0552  | `main`                               | `main.c`                   |
| 0x05A3  | `bscan_scan_slot_data`               | `bekant/bscan.c`           |
| 0x0605  | `bscan_scan_slot_rest`               | `bekant/bscan.c`           |
| 0x063D  | `bui_save_pos`                       | `bekant/bui.c`             |
| 0x0679  | `bui_init`                           | `bekant/bui.c`             |
| 0x0717  | `btn_timer` / `btn_gesture`          | `btn/btn.c`                |
| 0x0760  | `endstop_timer`                      | `bekant/endstop.c`         |
| 0x0D2D  | `endstop_init`                       | `bekant/endstop.c`         |

So the current build *appears* to be ivanwick + the
dual-gesture button improvement + what we believe to be the
recovered endstop detector. The endstop detector adds ~250
words of code (the `_endstop_*` and `bctrl_is_moving`/`max`/
`min` helpers). The ISR is at 0x0004 thanks to the
`__interrupt()` qualifier in `interrupts.c` — without it,
XC8 free mode would apparently dead-strip the function and
the interrupt vector would stay empty.

## What the original OEM hex appears to contain that ours does not

`pic-objdump`-equivalent on the original (the `gpdasm` output
in `disassembly/orginafirm_disasm.asm`) shows the following
additional code regions — *what we think* they are:

### Region 1: 0x1000–0x1043 — what looks like an 8-state jump table

A 33-entry **computed-goto** table indexed by state (0x4d).
Each entry is a 2-word `goto` to a state handler. This looks
like the dispatch core of the OEM's 8-state control machine.

```
0x1000:  goto 0x00cc        ; state 0 → default
0x1002:  goto 0x00d5        ; state 1 → handler
0x1004:  goto 0x00db        ; state 2 → handler
... (33 entries total)
0x1042:  goto 0x02b7        ; state 0x21 → handler
```

The state handlers appear to be at 0x00cc, 0x00d5, 0x00db,
0x00e1, 0x00e7, 0x00ed, 0x00f3, 0x00f9 (in 0x00xx — common
code), and at 0x0298, 0x029b, 0x029d, 0x029f, 0x02a1, 0x02a3,
0x02a5, 0x02a7, 0x02a9, 0x02ab, 0x02ad, 0x02af, 0x02b1, 0x02b3,
0x02b5, 0x02b7 (in 0x02xx).

### Region 2: 0x1046–0x10D7 — what looks like an encoder / EEPROM retlw table

A 141-entry lookup table addressed by `retlw` from a function
at 0x1046. The function adds 0xff to W, skips on borrow, then
indexes into PCLATH. Each entry is a 2-byte record (a few
are 3-byte). Decoded, this *looks like* a **structured memory
layout** — the first 13 entries would be a header (memory
size, version, etc.) and the remainder would be 8-byte or
4-byte records describing the EEPROM positions and their
default values.

### Region 3: 0x10D8–0x17FF — what we believe to be the state machine handler

~1,800 words of code that *appears* to implement the 8-state
control loop. This is **the largest single missing region**.
It seems to include:

- The state transition function at 0x10D8 (computed goto for
  state 0x4d).
- The "encoder change detector" at 0x05F3–0x0609 (used by the
  endstop code).
- A 50-tick debounce / wait loop at 0x1100–0x1165.
- The "compute new position" function at 0x063C–0x0672 —
  which we believe to be used for overshoot compensation
  when the user releases mid-move.
- The "send BCMD_STOP on endstop" handler at 0x0672–0x068D.

### Region 4: 0x1800–0x183F — what we believe to be a 32-entry LIN bus dispatch

A 32-entry dispatch table (computed goto) for what we believe
to be the LIN bus state machine. Each entry is a 2-word
`goto` to one of the LIN state handlers at 0x0022, 0x0024,
0x006c, 0x00c2, 0x0119, 0x0137, or one of the many 0x03xx,
0x04xx, 0x05xx, 0x06xx addresses. This *looks like* the LIN
protocol's state machine for sending and receiving frames.

The 0x1840–0x1FF9 region is **all 0x3fff (unprogrammed)** —
an earlier note in this project claimed it was an "8-state
button state machine" but that was a misreading. The actual
button handling appears to be integrated into the BCTRL
state machine at 0x05C0.

### Region 5: 0x1FFA–0x1FFF — what looks like an EEPROM copy helper

A 6-word function that appears to copy `WREG` bytes from
`INDF0` to `INDF1`. We believe this is the EEPROM-write
helper used by the factory-reset path.

```
0x1FFA:  movwf 0x7e        ; store count from W
0x1FFB:  moviw 0++         ; INDF0++
0x1FFC:  movwi 0x1++       ; INDF1++
0x1FFD:  decfsz 0x7e, 0x1  ; decrement, skip if 0
0x1FFE:  goto 0x1FFB       ; loop
0x1FFF:  retlw 0x00        ; return 0
```

### Region 6: 0x3800–0x3BE2 — what we believe to be an interrupt-driven 50 ms scheduler

~1,000 words of code that *appears* to:

- Implement the 50 ms tick that drives the OEM's state
  machine.
- Call the BCTRL state machine function at 0x05C0.
- Update the LIN bus schedule (which the BCTRL state machine
  consults to decide what to send).
- Handle the EEPROM read/write for the saved positions.

## Code we appear to be missing, in summary

| Region        | Words            | What we think it is                           |
| ------------- | ---------------- | --------------------------------------------- |
| 0x1000–0x1043 | 33               | 34-entry BCMD/state dispatch (jump table)     |
| 0x1046–0x10D7 | 141              | Encoder / EEPROM retlw lookup table           |
| 0x10D8–0x17FF | 1,800            | State machine handler (overshoot, endstop, …) |
| 0x1800–0x183F | 32               | 32-entry LIN bus dispatch                     |
| 0x1840–0x1FF9 | 0                | Unprogrammed (3fff)                           |
| 0x1FFA–0x1FFF | 6                | EEPROM copy helper                            |
| 0x3800–0x3BE2 | 1,000            | Interrupt-driven 50 ms scheduler              |
| **Total**     | **~3,000 words** | Roughly **1.5× the size of ivanwick**         |

## A possible misconception worth flagging

The ivanwick project's BCMD_* constants *appear* to be
**almost all wrong** (only `0xfc` for STOP looks like it
matches the OEM). The ivanwick code might work in practice
because the LIN leg controller is forgiving about which
BCMD it receives and may fall back to a sensible default.
If so, the current build's BCTRL layer would also be
**functionally correct for the simple cases** (idle, stop)
but may misbehave on edge cases (overshoot,
anti-collision). We have not verified this.

The **one** BCMD that *appears* to be required to be exact
is `0xfc` (STOP) — and that one *does* match. The bug in
[issue #4](https://github.com/ivanwick/bekantfirmware/issues/4)
therefore *appears* to be **not** a BCMD bug, but rather a
missing endstop detector that watches the encoder. The
reconstructed `endstop.c` in `./bekant/endstop.c` is what
we believe to be the fix (also copied into
`src/bekant/endstop.c` where it is wired into the main
build), and what we believe to be the equivalent logic is
present in the OEM firmware at 0x0672–0x068D.
