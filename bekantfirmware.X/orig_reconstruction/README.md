# Original IKEA BEKANT firmware — C reconstruction

This folder is a **best-effort reverse-engineered C reconstruction** of the
firmware inside `orginafirm.hex` (the file ivanwick released at
https://github.com/ivanwick/bekantfirmware/files/7874509/orginafirm.hex.zip).

The reconstruction is meant as a **reference for future work**, not as a
buildable firmware. The original Intel HEX and the disassembly are
included in `disassembly/` for ground truth.

## Layout

```
orig_reconstruction/
├── README.md                   ← this file
├── main.c                      ← startup (reset vector → main)
├── configuration_bits.c        ← __CONFIG settings recovered from the HEX
├── system.c / system.h         ← oscillator / port setup
├── user.c / user.h             ← USART / LIN init (analogous to ivanwick)
├── interrupts.c                ← ISR dispatch
├── btn/
│   ├── btn.c                   ← button state machine (simplified)
│   └── btn.h
├── lin/
│   ├── lin_d.c                 ← LIN driver (kept close to ivanwick)
│   └── lin_d.h
├── bekant/
│   ├── bscan.c / bscan.h       ← startup bus scan (kept close to ivanwick)
│   ├── bctrl.c / bctrl.h       ← LIN control plane (recovered, see notes)
│   ├── bui.c / bui.h           ← upper-level UI (best-effort)
│   └── orig_endstop.c          ← recovered endstop detector
├── disassembly/
│   ├── orginafirm.hex          ← original Intel HEX
│   ├── orginafirm_disasm.asm   ← gpdasm output
│   └── orginafirm_disasm_nos.asm ← gpdasm with labels + chip headers
```

## How the reconstruction was done

1. `gpdasm -p 16F1938 -nos orginafirm.hex` produced the disassembly in
   `disassembly/`.
2. Function entry points were identified by walking `call` targets and
   removing all destinations that aren't real function starts.
3. Each function was mapped to its closest ivanwick equivalent by
   matching state-machine shape, register names, and constant literals.
4. The BCMD_* constants (LIN command bytes) were recovered from the
   `movlw` table at **0x0d5–0x0ff** in the disassembly, which is a
   state-indexed lookup written to register `0x61`.
5. The factory-reset sequence was recovered from the state machine at
   **0x05c0–0x068d** (state register `0x4d`).
6. The endstop / over-travel stop sequence was recovered from the same
   state machine at **0x0672–0x068d** (state 0x07 → BCMD_STOP 0xfc).

## Confidence levels

| File                  | Confidence | Notes                                                                                          |
|-----------------------|------------|------------------------------------------------------------------------------------------------|
| `main.c`              | High       | Reset vector / interrupt vector / startup chain is unambiguous (`0x0000 → 0x001f → 0x0221 → 0x0185 → 0x0190`). |
| `configuration_bits.c`| High       | Recovered from the `__config` word in the HEX.                                                  |
| `system.c`            | Medium     | Oscillator values are guesses based on register-init patterns in the disassembly.              |
| `user.c`              | Medium     | USART init registers match the ivanwick project's USART setup; little new to discover.        |
| `interrupts.c`        | High       | Three IRQs observed in the disassembly: USART RX, Timer2, Timer4.                              |
| `btn/btn.c`           | Low        | The button state machine in the OEM firmware is much more complex than ivanwick's. We provide a simplified version that captures the visible behavior. |
| `lin/lin_d.c`         | Medium     | LIN protocol handling is nearly identical to ivanwick — we keep the same code.                 |
| `bekant/bscan.c`      | High       | Startup bus scan uses the same 0x3c/0x3d LIN IDs and slot sequence.                              |
| `bekant/bctrl.c`      | Low        | BCMD_* values are recovered (see below). The state machine layout is best-effort.               |
| `bekant/bui.c`        | Low        | The high-level UI is heavily guessed; the OEM firmware's user-facing model is not 1:1 with ivanwick's. |
| `bekant/orig_endstop.c` | High     | The endstop detection is the most interesting and best-understood piece — see ENDSTOP_ANALYSIS.md. |

## BCMD values recovered from the disassembly

The OEM firmware's command bytes are written to register `0x61` (in RAM
bank 2) by the state-indexed lookup at **0x0d5–0x0ff** in the
disassembly. The mapping is:

| State (`0x52`) | OEM BCMD byte | Comment                                                |
|----------------|---------------|--------------------------------------------------------|
| 0, 1 (default) | `0x50`        | Idle / stop — desk is not moving                       |
| 2              | `0x49`        | Initialized move / "wake" the bus                      |
| 3              | `0xca`        | Move up (fast)                                         |
| 4              | `0x8b`        | Move up (slow)                                         |
| 5              | `0x4c`        | Move down (slow)                                       |
| 6              | `0x0d`        | Move down (fast)                                       |
| 7              | `0x8e`        | Decelerate (overshoot)                                 |
| 8              | `0xcf`        | Pre-stop / "almost there"                              |
| —              | `0xfc`        | STOP (used in the endstop-detector at 0x0678)           |

Compare with ivanwick's `bctrl.c`:

| ivanwick name        | ivanwick value | OEM value | Match? |
|----------------------|----------------|-----------|--------|
| `BCMD_STOP_IDLE`     | `0xfc`         | `0xfc`    | ✓      |
| `BCMD_AFTER_SCAN_INIT` | `0xbf`       | `0x50` (state 0 default) | ✗ |
| `BCMD_UP`            | `0x86`         | `0xca` / `0x8b` (two-speed) | ✗ |
| `BCMD_DOWN`          | `0x85`         | `0x4c` / `0x0d` (two-speed) | ✗ |
| `BCMD_DECEL`         | `0x87`         | `0x8e`    | ✗      |
| `BCMD_PRE_STOP`      | `0x84`         | `0xcf`    | ✗      |
| `BCMD_ERR_UP`        | `0xbc`         | `0xca`/?? | ✗      |
| `BCMD_ERR_DOWN`      | `0xbd`         | `0x4c`/?? | ✗      |
| `BCMD_PRE_MOVE`      | `0xc4`         | `0x49`    | ✗      |

**Conclusion:** the ivanwick project's BCMD_* constants are largely
guesses that happen to work for the `0xfc` STOP command. The other
constants are wrong. The ivanwick project got the desk to work
*despite* this — likely because the LIN slave (the leg controller) is
forgiving about which BCMD it receives and falls back to a sensible
default. But the **endstop detection** specifically depends on
sending `0xfc` to stop the legs cleanly, which the ivanwick project
**does** do correctly (because `BCMD_STOP_IDLE` matches). The bug
described in issue #4 is therefore NOT in the BCMD choice.

See `docs/ENDSTOP_ANALYSIS.md` for the full root-cause analysis.
