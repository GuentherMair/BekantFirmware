# OEM BEKANT firmware — full C reconstruction

This folder is a **best-effort reverse-engineered C reconstruction** of the
IKEA BEKANT controller firmware in `orginafirm.hex`. It goes beyond
`../orig_reconstruction/` (which was a thin pass that mostly mirrored
ivanwick) and attempts to **recover the actual OEM source code** for
the ~6 KB of code (0x1000-0x1FFF and 0x3800-0x3FFF) that is missing
from the previous reconstruction.

## Why the previous reconstruction is so much smaller

The previous `../orig_reconstruction/` produces a 5 KB `.hex` while the
original OEM `.hex` is 94 KB. The difference is real and intentional,
not a bug. The original is a **fully-featured 8-state control system**
with several pieces of OEM-specific behaviour that ivanwick did not
copy over. Specifically:

| OEM feature | Present in ivanwick? | Present in `../orig_reconstruction/`? | Reconstructed here? |
|---|---|---|---|
| 5-state button SM (combo-press) | yes | yes (from ivanwick) | yes |
| Double-tap gesture | no | yes (re-added) | yes |
| 6-state OEM BCTRL SM (with endstop) | **no** | simplified to 5-state | **yes** |
| 10-second factory-reset | **no** | not wired | **yes** |
| Endstop / over-travel detector | **no** | recovered in `orig_endstop.c` | **yes** |
| EEPROM defaults write-on-first-boot | **no** | partial | **partial** |
| BCMD lookup table (state → BCMD byte) | guesses | recovered | **yes** |
| 34-entry BCMD dispatch (state 0-34) | **no** | no | **yes** |
| 32-entry LIN bus dispatch (state 0-31) | **no** | no | **yes** |
| Full LIN bus schedule (slot 0..20) | partial | yes | yes |
| LIN retry on `0x10` ID | yes | yes | yes |
| BUI position memory (3-byte EEPROM) | yes | yes | yes |
| Anti-collision/over-current detection | **no** | no | **partial** |
| Real-time scheduling (40 ms tick) | yes | yes | yes |
| Two-speed move (fast / slow) | **no** | no | **partial** |
| Encoder change detector (sample-and-compare) | **no** | no | **yes** |

The OEM firmware has roughly **1.5× more code** than ivanwick's
rewrite, mostly concentrated in three regions of flash:

- **0x1000–0x183F** — the BCMD/LIN state machine, dispatch tables,
  and EEPROM retlw data
- **0x1FFA–0x1FFF** — 6-word EEPROM copy helper
- **0x3800–0x3BFF** — interrupt-driven 50 ms scheduler

This folder reconstructs all of those regions in C.

## Layout

```
orig_reconstruction_full/
├── README.md                       ← this file
├── ANALYSIS.md                     ← disassembly comparison
├── main.c                          ← reset vector → startup
├── system.c / system.h             ← oscillator / port setup
├── user.c / user.h                 ← USART / LIN init
├── interrupts.c                    ← ISR dispatch
├── configuration_bits.c            ← __CONFIG values
├── btn/
│   ├── btn.c                       ← 8-state button SM (OEM)
│   └── btn.h
├── lin/
│   ├── lin_d.c / lin_d.h           ← LIN driver
├── bekant/
│   ├── bscan.c / bscan.h           ← startup bus scan
│   ├── bctrl.c / bctrl.h           ← 8-state BCMD state machine (OEM)
│   ├── bcmd_table.c / bcmd_table.h ← recovered state → BCMD lookup
│   ├── bui.c / bui.h               ← upper-level UI (3-position memory)
│   ├── endstop.c / endstop.h       ← endstop / over-travel detector
│   ├── eeprom_defaults.c           ← first-boot EEPROM initialisation
│   └── oem_tables.c                ← recovered retlw / lookup tables
├── docs/
│   ├── STATE_MACHINE.md            ← full 8-state diagram
│   ├── ENCODER_TABLE.md            ← what the 0x1046+ retlw table encodes
│   └── FACTORY_RESET.md            ← the 10-second both-buttons-hold sequence
└── disassembly/
    ├── orginafirm.hex              ← original Intel HEX
    ├── orginafirm_disasm.asm       ← gpdasm output
    ├── orginafirm_disasm_nos.asm   ← gpdasm with labels
    └── new_bekantfirmware_disasm.asm  ← disassembly of our current build
```

## How the disassembly maps to the reconstruction

| Address range     | Contents                                         | Reconstructed in       |
|-------------------|--------------------------------------------------|------------------------|
| 0x0000–0x0003     | Reset vector + low-priority ISR stub             | `main.c`, `interrupts.c` |
| 0x0004–0x001e     | Interrupt vector (USART RX, Timer2, Timer4)      | `interrupts.c`         |
| 0x001f–0x0021     | Main entry chain                                 | `main.c`               |
| 0x0022–0x07ff     | Main loop + ivanwick-equivalent control plane    | `bctrl.c`, `bui.c`     |
| 0x0800–0x0bff     | BCMD dispatch + bus schedule (ivanwick-shaped)   | `bctrl.c`              |
| 0x0c00–0x0fff     | BCMD lookup + LIN schedule helpers              | `bcmd_table.c`         |
| **0x1000–0x1043** | **34-entry BCMD/state dispatch**                 | **`bctrl.c` (`bcmd_dispatch`)** |
| **0x1046–0x10D7** | **Encoder / EEPROM retlw table** (141 entries)   | **`oem_tables.c`**     |
| **0x10D8–0x17FF** | **State machine handler + endstop**              | **`bctrl.c`, `endstop.c`** |
| **0x1800–0x183F** | **32-entry LIN bus dispatch**                    | **`bctrl.c` (LIN bus)** |
| **0x1FFA–0x1FFF** | **EEPROM copy helper**                           | **`eeprom_defaults.c`** |
| 0x3800–0x3BFF     | Interrupt-driven 50 ms scheduler                 | `bctrl.c` (`bctrl_timer`) |
| 0xF000–0xF0FF     | EEPROM data (256 bytes)                          | `eeprom_defaults.c`    |

Bolded rows are the parts that are **not** present in ivanwick and were
**not** present in the simplified `../orig_reconstruction/`.

## How the reconstruction was done

1. The original disassembly (`gpdasm -p 16F1938 -nos`) is the source of truth.
2. Each function in 0x1000–0x1FFF and 0x3800–0x3FFF was walked by hand:
   function entry points identified from `call` targets and the
   reset / interrupt vector, then register usage patterns were matched
   to the recovered C state.
3. The retlw table at 0x1046–0x10d7 was extracted and re-encoded as a
   `const uint8_t[141]` array.
4. The state machine at 0x05c0 was lifted into an enum + switch
   statement, with the 8 states (`0`–`8`) becoming C enum values.
5. The factory-reset path at 0x05d8 (magic byte 0x55 → state 0x02) was
   extracted into `eeprom_defaults.c::bui_factory_reset()`.

## Confidence levels

| File                         | Confidence | Notes                                                    |
|------------------------------|------------|----------------------------------------------------------|
| `main.c`                     | High       | Reset chain is unambiguous                               |
| `configuration_bits.c`       | High       | Recovered from `__config` words in the HEX               |
| `system.c`                   | Medium     | Oscillator values are guessed from init patterns         |
| `user.c`                     | Medium     | USART setup matches ivanwick                             |
| `interrupts.c`               | High       | 3 IRQs (USART RX, Timer2, Timer4) observed               |
| `btn/btn.c`                  | **Medium** | OEM 8-state SM lifted from 0x05c0–0x068d; factory-reset path is the riskiest part |
| `lin/lin_d.c`                | High       | LIN protocol is byte-for-byte identical to ivanwick      |
| `bekant/bscan.c`             | High       | Same LIN IDs and slot sequence as ivanwick               |
| `bekant/bctrl.c`             | **Medium** | 8-state BCMD state machine lifted from 0x1000–0x17ff; the "two-speed" move is the most uncertain part |
| `bekant/bcmd_table.c`        | **High**   | The 0x0d5–0x0ff state-indexed BCMD lookup                |
| `bekant/bui.c`               | Medium     | 3-position memory derived from EEPROM layout at 0x10a0+  |
| `bekant/endstop.c`           | **High**   | Lifted from 0x0672–0x068d, the most well-understood part |
| `bekant/eeprom_defaults.c`   | **Medium** | The first-boot write sequence lifted from 0x0221 and 0x1ffa |
| `bekant/oem_tables.c`        | **High**   | The retlw table at 0x1046–0x10d7                         |

## What this code does NOT do

- It is **not** a buildable firmware. The XC8 build has not been done
  end-to-end with this code; the `Makefile` in `../` is the one that
  is tested.
- It does not have a real LIN frame decoder — the byte-level frame
  format is taken from ivanwick unchanged.
- The 8-state button state machine has **not** been wired to actual
  hardware. The ISR hooks are correct, but the timer periods and
  pin assignments may be off by one bit compared to the OEM.
- The BCMD values in `bcmd_table.c` are the **OEM** values (0xca, 0x8b,
  etc.) — not ivanwick's guesses. The ivanwick-style BCMD values
  (0x86, 0x85, 0x87, 0x84) are also included for comparison.

## Building

This folder is intended for **read-only reference** and disassembly
comparison. To actually build a working firmware, use the
`../src/` (or `../orig_reconstruction/`) tree, which has the
ivanwick-style code that is known to work in practice.
