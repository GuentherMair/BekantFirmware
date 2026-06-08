# The 10-second both-buttons-hold factory reset

The OEM firmware supports a **factory-reset** sequence:
holding both buttons for 10 seconds clears the saved
positions and rewrites the EEPROM defaults.

## The gesture

The user holds both `△` and `▽` buttons for 10 seconds.
After 10 seconds, the saved positions are reset to the
OEM defaults (`0x0021`, `0x00a0`, `0x0120`, `0x01d2`) and
the controller emits a single "click" of the relay as
feedback.

## The state machine

The factory-reset path uses the BCTRL state machine at
0x05C0. Specifically:

- The button handler in `btn/btn.c` increments a counter
  `0x42` while both buttons are held.
- After 10 seconds (200 ticks at 50 ms per tick, the
  exact threshold is recovered from the disassembly at
  0x05DD), the button handler writes the magic byte
  `0x55` to RAM byte `0x4c` and sets the BCTRL state
  to 1.
- The BCTRL state machine reads `0x4c` at 0x05D8:
  - If `0x4c == 0x55`, state := 2 and start a counter.
  - If `0x4c != 0x55`, state := 0 and clear 0x53.
- State 2 (factory-reset armed) at 0x05EE:
  - Clear 0x4d (state := 0).
  - Set bit 0x2B.4 (a "reset done" flag).
  - Read 0x4c and call 0x00D3 (the BCMD dispatcher).
- The BCMD dispatcher at 0x00D3 writes 0x49 (BCMD
  state 2 = init) to register 0x61, which the LIN
  driver sends on the next bus frame.
- The next state machine tick rewrites the EEPROM
  defaults via the function at 0x0221.

## The button counter

The button counter at `0x42` is the 10-second timer.
Looking at the disassembly:

- 0x05DD: `decfsz 0x42, 0x0` — decrement 0x42, skip if zero
- 0x05E0: `return` — if not zero, return (wait another tick)
- 0x05E1-0x05E6: if zero, write 0x1a to SFR, clear 0x42, return

So the counter is **decremented** each tick (50 ms), and
the magic byte is processed when it reaches 0. The counter
is initialised to 200 (10 seconds × 20 Hz = 200 ticks) by
the button handler.

## The EEPROM defaults

The four default positions written by the factory-reset:

| Address | Value  | ivanwick equivalent |
|---------|--------|---------------------|
| 0x00    | 0x0021 | 0x0636 (sitting)    |
| 0x02    | 0x00a0 | -                   |
| 0x04    | 0x0120 | -                   |
| 0x06    | 0x01d2 | 0x1600 (standing)   |

The ivanwick firmware only stores two positions (sitting
and standing), so the OEM firmware has **two extra
positions** stored in EEPROM. These are not used by
ivanwick's BUI.

The ivanwick defaults are deliberately different from the
OEM defaults:
- 0x0636 is the typical sitting height (around 70 cm).
- 0x1600 is the typical standing height (around 110 cm).
- 0x0021 / 0x01d2 are the OEM defaults, which are at the
  **end stops** of the desk's range.

ivanwick's defaults assume the user has already moved the
desk to the desired positions and saved them with the
3-second both-buttons-hold gesture. The OEM defaults put
the desk at the physical endstops, which is the correct
behaviour for a factory-reset.

## What the C reconstruction does

The `bekant/eeprom_defaults.c` file has the `bui_factory_reset()`
function that performs the actual EEPROM rewrite. It is
called from the BUI layer when the factory-reset path is
detected (in the OEM firmware, this is the state machine
transition from state 1 to state 2 in the 0x05C0 dispatcher).

The `btn/btn.c` (in this folder) is responsible for
detecting the 10-second both-buttons-hold gesture and
writing the magic byte `0x55` to RAM. The C code does
this by:

1. Counting ticks while both buttons are held.
2. When the count reaches 200 (10 seconds), write 0x55
   to `bctrl_factory_magic` and set the BCTRL state to 1.
3. The BCTRL state machine then handles the actual reset.

## What's not implemented

The C reconstruction does **not** wire the button handler
to the BUI layer for the factory-reset. The ivanwick-style
`btn/btn.c` in `../src/btn/btn.c` (and the unified variant
in this folder's `btn/btn.c`) treats the 3-second
both-buttons-hold as the "save" gesture and does not
distinguish a 10-second hold. To fully support the
OEM factory-reset, the button handler would need a second
threshold of 200 ticks (10 seconds) that triggers
`bui_factory_reset()` instead of the regular save.

## See also

- `bekant/eeprom_defaults.c::bui_factory_reset()` — the
  reset function
- `bekant/bctrl.c::bctrl_state_machine()` — the state
  machine that handles the reset
- `docs/STATE_MACHINE.md` — the full state diagram
