# BEKANT firmware — combined project & reverse engineering deliverable

This deliverable contains:

1. A **combined firmware project** that supports both upstream
   gesture variants (ivanwick combo-press and DieSteinhose
   double-tap) **in parallel** from a single build.
2. A **best-effort C reconstruction** of the original IKEA firmware
   (from `orginafirm.hex`) inside `orig_reconstruction/`.
3. An **endstop / over-travel analysis** that explains the root
   cause of [ivanwick/bekantfirmware issue #4](https://github.com/ivanwick/bekantfirmware/issues/4)
   and proposes a fix.

## Quick links

- [`README.md`](README.md) — top-level guide
- [`USAGE.md`](USAGE.md) — end-user button reference
- [`docs/COMPARISON.md`](docs/COMPARISON.md) — ivanwick vs DieSteinhose
- [`docs/TRACE.md`](docs/TRACE.md) — state-machine traces
- [`docs/ENDSTOP_ANALYSIS.md`](docs/ENDSTOP_ANALYSIS.md) — endstop bug analysis
- [`orig_reconstruction/`](orig_reconstruction/) —
  reconstructed C code of the original IKEA firmware
- [`orig_reconstruction/disassembly/`](orig_reconstruction/disassembly/) —
  the disassembly files (`.hex` and two `.asm` variants) for reference

## Tree

```
BekantFirmware/
├── README.md                                       ← top-level
├── USAGE.md                                        ← end-user button reference
├── docs/
│   ├── COMPARISON.md
│   ├── DELIVERABLE.md                              ← this file
│   ├── ENDSTOP_ANALYSIS.md
│   └── TRACE.md
├── orig_reconstruction/                            ← best-effort C reconstruction
│   ├── README.md
│   ├── ANALYSIS.md
│   ├── main.c
│   ├── configuration_bits.c
│   ├── system.c
│   ├── system.h
│   ├── user.c
│   ├── user.h
│   ├── interrupts.c
│   ├── test_sfr_stubs.h
│   ├── test_sfr_stubs.c
│   ├── bekant/
│   │   ├── bctrl.c                                 ← uses recovered OEM BCMD values
│   │   ├── bctrl.h
│   │   ├── bscan.c
│   │   ├── bscan.h
│   │   ├── bui.c
│   │   ├── bui.h
│   │   ├── bcmd_table.c / bcmd_table.h              ← recovered state → BCMD lookup
│   │   ├── oem_tables.c                             ← recovered retlw / lookup tables
│   │   ├── eeprom_defaults.c                        ← first-boot EEPROM init
│   │   └── endstop.c                                ← RECOVERED endstop detector
│   ├── btn/
│   │   ├── btn.c
│   │   └── btn.h
│   ├── lin/
│   │   ├── lin_d.c
│   │   └── lin_d.h
│   └── disassembly/
│       ├── orginafirm.hex                          ← original Intel HEX
│       ├── orginafirm_disasm.asm                   ← gpdasm output
│       └── orginafirm_disasm_nos.asm               ← gpdasm with labels
└── src/
    ├── README-ORIGINAL.md                          ← preserved from ivanwick
    ├── main.c
    ├── configuration_bits.c
    ├── system.c
    ├── system.h
    ├── user.c
    ├── user.h
    ├── interrupts.c
    ├── test_sfr_stubs.h                            ← for syntax-only checks
    ├── test_sfr_stubs.c
    ├── Makefile
    ├── nbproject/                                  ← preserved from ivanwick
    ├── bekant/
    │   ├── bctrl.c                                 ← now uses recovered OEM BCMD values
    │   ├── bctrl.h                                  ← BCMD values replaced with OEM ones
    │   ├── bscan.c
    │   ├── bscan.h
    │   ├── bui.c
    │   ├── bui.h
    │   ├── endstop.c                                ← RECOVERED endstop detector, wired in
    │   └── endstop.h
    ├── btn/
    │   ├── btn.c                                   ← REWRITTEN: unified state machine
    │   └── btn.h                                   ← REWRITTEN: documents both gestures
    └── lin/
        ├── lin_d.c
        └── lin_d.h
```

## How the unified gesture works

The `btn/btn.c` state machine accepts **either** encoding for the
memory-position gesture:

- **Combo-press** (from ivanwick): hold `△`, tap `▽` while still
  holding `△` → `INPUT_MEM_UP`. While in `INPUT_UP`, a press of
  `▽` jumps straight to `MEM_UP`.
- **Double-tap** (from DieSteinhose): tap-tap `△` within 500 ms →
  `INPUT_MEM_UP`. Releasing `△` arms a 10-tick window; a second
  `△` press inside the window fires `MEM_UP`.

Both paths emit the same `INPUT_MEM_UP` event. The downstream BUI
does not care which the user used. A new user can pick whichever
feels natural; an existing user keeps their muscle memory.

Build command (single, no flag required):

```sh
xc8 --chip=PIC16LF1938 --std=c99 --runtime=+clear \
    src/main.c \
    src/system.c \
    src/user.c \
    src/configuration_bits.c \
    src/interrupts.c \
    src/btn/btn.c \
    src/lin/lin_d.c \
    src/bekant/bscan.c \
    src/bekant/bctrl.c \
    src/bekant/bui.c \
    src/bekant/endstop.c
```

For step-by-step build + flash instructions, see
[`README.md`](README.md). For the end-user button reference, see
[`USAGE.md`](USAGE.md).

## How the endstop fix was integrated

The recovered endstop detector is wired into the main build via
three small additions in `src/`:

1. `src/bekant/endstop.c` and `src/bekant/endstop.h` — the
   detector itself, copied from `orig_reconstruction/bekant/`.
2. `src/user.c::InitApp()` — registers the endstop callback:
   ```c
   endstop_report = bctrl_stop_if_at_endstop;
   endstop_init();
   ```
   The same pattern that the project already uses to register
   `bctrl_report_pos = bui_set_pos`.
3. `src/bekant/bctrl.c::bctrl_timer()` — calls
   `endstop_timer()` once per scheduler slot, before
   `bctrl_report_pos`.

The OEM BCMD values from the recovered `bcmd_table.c` are also
inlined into `src/bekant/bctrl.h`; the build's BCMD_* constants
match the OEM (0x50/0x49/0xca/0x8b/0x4c/0x0d/0x8e/0xcf/0xfc)
rather than ivanwick's guesses (0x86/0x85/0x87/0x84/0xc4/0xbf).
In practice the LIN leg controller is forgiving so ivanwick's
guesses work too, but the OEM values are the right ones to use.

The endstop fix does **not** depend on the BCMD values being
exact — the fix specifically issues the universal `0xfc` STOP,
which ivanwick already had right.

## Verification status

The combined project builds clean with XC8 v3.10 free mode under
`make`, producing `dist/bekantfirmware.hex` (15,476 bytes,
2,797 words of 16,384 used = 17.1%). The disassembly
(`pic-objdump -d dist/bekantfirmware.elf`) shows the recovered
endstop detector at `_endstop_timer` (0x0760) and the
ISR at `_isr` (0x0004) wired to `_lin_txrx_daemon`,
`_bctrl_timer`, and `_btn_timer` per the PIR bits.

The `test_sfr_stubs.h` shim is kept for `gcc -fsyntax-only`
checks; the real build uses the real XC8 headers from the DFP.

## What is left for future work

- **No PIC simulation or hardware test.** A real run on a
  PIC16LF1938 simulator or actual hardware is the final
  validation step. The endstop detector was hand-translated
  from the disassembly, so the threshold and behavior should
  be re-verified once a unit is available.
- **The reconstruction's btn.c is a simplified version.** The full
  8-state OEM firmware state machine at disassembly 0x05c0 is much
  more elaborate than ivanwick's; we kept the simplified version
  because the extra states are for the 10-second factory-reset
  sequence, which is a separate (downstream) feature.
- **Factory-reset is not wired into `btn/btn.c`.** The recovered
  `bui_factory_reset()` lives in `orig_reconstruction/bekant/`
  but is not connected to the unified button state machine. The
  endstop fix and the factory-reset are independent features,
  and the endstop fix is the higher-priority one.
