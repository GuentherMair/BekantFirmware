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
bekant-compound/
├── README.md                                       ← top-level
├── USAGE.md                                        ← end-user button reference
├── docs/
│   ├── COMPARISON.md
│   ├── DELIVERABLE.md                              ← this file
│   ├── ENDSTOP_ANALYSIS.md
│   └── TRACE.md
├── orig_reconstruction/                            ← best-effort C reconstruction
│   ├── README.md
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
│   │   └── orig_endstop.c                          ← RECOVERED endstop detector
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
    │   ├── bctrl.c                                 ← unchanged from ivanwick
    │   ├── bctrl.h
    │   ├── bscan.c
    │   ├── bscan.h
    │   ├── bui.c                                   ← unchanged from ivanwick
    │   └── bui.h
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
    src/bekant/bui.c
```

For step-by-step build + flash instructions, see
[`README.md`](README.md). For the end-user button reference, see
[`USAGE.md`](USAGE.md).

## How to apply the endstop fix to ivanwick

Three small changes:

1. Copy `orig_reconstruction/bekant/orig_endstop.c` and
   `bekant/orig_endstop.h` into the ivanwick project.
2. In `main.c`, add `orig_endstop_init();` next to `bui_init()`.
3. In `user.c::InitApp()`, add the line
   `orig_endstop_report = bctrl_stop_if_at_endstop;`
   (this is exactly the same pattern that the project already uses to
   register `bctrl_report_pos = bui_set_pos`).

No other file needs to be modified.

## Verification status

The combined project compiles clean using `gcc -fsyntax-only` with
the bundled `test_sfr_stubs.h` shim. The XC8 compiler is required
for a real build — it is not available in this environment. The
reconstruction also compiles clean under the same conditions.

## What I did not do

- **No real XC8 build.** The XC8 compiler is not available in this
  environment, so a real `.hex` output is not produced. The code
  compiles to syntax-clean C.
- **No PIC simulation.** A real run on a PIC16LF1938 simulator or
  hardware would be the final validation step.
- **The reconstruction's btn.c is a simplified version.** The full
  8-state OEM firmware state machine at disassembly 0x05c0 is much
  more elaborate than ivanwick's; we kept the simplified version
  because the extra states are for the 10-second factory-reset
  sequence, which is a separate (downstream) feature.
- **BCMD_* values for ivanwick were not corrected.** ivanwick's
  guesses mostly work in practice because the leg controller is
  forgiving, and changing them is risky. The fix for issue #4
  does not require BCMD changes — the universal `0xfc` STOP is
  already correct in ivanwick.
