# The 8-state button / BCTRL state machine

The OEM firmware has a unified state machine that handles both
button presses and the BCTRL control plane. The state register
is RAM byte `0x4d` (we model this as `bctrl_state` in
`bekant/bctrl.c`).

## State diagram

```
                    ┌──────────────────────────────────────────┐
                    │                                          │
                    ▼                                          │
            ┌──────────────┐                                   │
            │ State 0      │  (default)                        │
            │ IDLE         │                                   │
            └──────┬───────┘                                   │
                   │                                           │
       ┌───────────┼─────────────┬───────────────┐             │
       │           │             │               │             │
       ▼           ▼             ▼               ▼             │
   factory-    encoder       encoder         default         │
   reset       pre-check     endstop        (no-op)         │
   trigger     (sample)      check                          │
   (state 1)   (state 3)     (state 4)                      │
       │           │             │                           │
       │           ▼             ▼                           │
       │    ┌─────────────┐  ┌─────────────┐                 │
       │    │ State 3     │  │ State 4     │                 │
       │    │ encoder     │─▶│ sample-     │                 │
       │    │ pre-check   │  │ and-compare │                 │
       │    └─────────────┘  └──────┬──────┘                 │
       │                            │                        │
       │              ┌─────────────┴────────────┐           │
       │              ▼                          ▼           │
       │       encoder equal             encoder changed    │
       │       (stall detected)          (still moving)     │
       │              │                          │           │
       │              ▼                          └─┐         │
       │       ┌─────────────┐                    │         │
       │       │ State 6     │  send BCMD_STOP   │         │
       │       │ ENDSTOP     │  (0xfc)            │         │
       │       │ detected    │◀───────────────────┘         │
       │       └──────┬──────┘                              │
       │              │                                     │
       ▼              ▼                                     │
   ┌─────────────┐  return to IDLE                          │
   │ State 1     │                                          │
   │ factory-    │  check magic byte 0x55                   │
   │ reset       │                                          │
   │ trigger     │                                          │
   └──────┬──────┘                                          │
          │ 0x55 seen                                       │
          ▼                                                 │
   ┌─────────────┐                                          │
   │ State 2     │                                          │
   │ factory-    │  re-initialise EEPROM defaults           │
   │ reset       │  send STOP                               │
   │ armed       │                                          │
   └──────┬──────┘                                          │
          │                                                 │
          └─────────────────────────────────────────────────┘
                                │
                                ▼
                          return to IDLE
```

## States in detail

### State 0 — IDLE (default)

The default state. The BCTRL state machine does nothing; the
firmware polls buttons and waits for user input.

In the disassembly at 0x05C0-0x05C7, the xor chain test for
state==0 falls through to the default handler at 0x068E
(which is just `return`).

### State 1 — factory-reset trigger

The button handler increments a counter while both buttons are
held. After 10 seconds, the counter overflows and the handler
writes the magic byte `0x55` to RAM byte `0x4c` and sets the
BCTRL state to 1.

The BCTRL state machine at 0x05D8 reads RAM byte `0x4c`:

- If `0x4c == 0x55` → state := 2, start a counter.
- If `0x4c != 0x55` → state := 0, clear 0x53 (set to 1).

### State 2 — factory-reset armed

The transition from state 1. At 0x05EE-0x05F2, the handler:
- Clears 0x4d (state := 0).
- Sets bit 0x2B.4 (a "reset done" flag).
- Reads 0x4c and jumps to 0x00D3 (the BCMD dispatcher).

The BCMD dispatcher at 0x00D3 writes the BCMD byte to register
0x61, which the LIN driver sends on the next bus frame.

### State 3 — encoder pre-check

The start of the endstop detection flow. At 0x05F3-0x0609:
- Store `0xe3` in 0x6d (a counter).
- Subtract from 0x4e.
- Read 0x46, add to 0x6d, store in 0x6e.
- Read 0x4c into 0x01.
- Set 0x4b bit 1, clear 0x48.
- Decrement 0x4e; if zero, state := 4.

This sets up the encoder sample-and-compare logic.

### State 4 — encoder endstop check

The endstop detection logic. At 0x060A-0x0671:
- Read 0x4c into 0x6b.
- If 0x4f == 0x14, copy 0x62 to 0x6d (special case).
- Clear 0x6b, 0x6c.
- Read 0x46, store in 0x6d.
- Compare 0x6e to 0x6c, then 0x6d to 0x6b. If both equal,
  encoder has stopped advancing.
- Increment 0x6b, then 0x6c on overflow.
- If Z flag set, branch to 0x063C.
- At 0x063C: complement 0x6d (compute NOT).
- Compare 0x6e to 0x6e (always equal!), then 0x6d to 0x6d.
- If Z flag set, branch to 0x065F (no endstop).
- Otherwise: state := 0, clear 0x53, increment to 1.
- Check 0x44 == 2: if so, branch to 0x065A.
- If 0x42 == 3, state := 0, 0x44 := 3.
- If 0x44 == 2, branch to 0x065A.

The endstop is detected when the encoder reading does not
change between two consecutive samples. After `ENDSTOP_STALL_TICKS`
(70, i.e. 350 ms at 5 ms per tick) of being stuck, the
state machine transitions to state 6.

### State 5 — (unused)

State 5 is reserved for OEM-specific behaviour and is not
tested in the 0x05C0 xor chain. It falls through to the
default handler at 0x068E (return).

### State 6 — ENDSTOP detected → send BCMD_STOP

The endstop-handled state. At 0x0672-0x0683:
- Read 0x2A, XOR with 0x4c.
- If equal (encoder reading has not changed), branch to 0x0684.
- If different: write 0xFC (universal STOP) to W, store in 0x61.
- Clear 0x4D (state := 0).
- Clear 0xC7 (moving flag).
- Clear 0x53, increment to 1.
- Decrement 0x44, return if not zero.
- If 0x44 reaches 0, check 0x42: if 0x42 == 2, branch to 0x065A.

This is the missing piece that fixes [issue #4](https://github.com/ivanwick/bekantfirmware/issues/4)
in ivanwick. The `bctrl_stop_if_at_endstop()` function in
`bekant/bctrl.c` is the C equivalent.

### States 7, 8+ — (unused)

States 7 and above are not tested in the 0x05C0 xor chain.
They fall through to the default handler at 0x068E (return).

## Transitions summary

| From state | To state | Trigger                           |
|------------|----------|-----------------------------------|
| 0          | 1        | 10 s both-buttons hold            |
| 0          | 3        | User starts moving (UP/DOWN)      |
| 1          | 2        | Magic byte 0x55 seen              |
| 1          | 0        | No magic (timeout or release)     |
| 2          | 0        | Reset done, send STOP             |
| 3          | 4        | Counter reaches 0                 |
| 3          | 0        | User releases buttons             |
| 4          | 6        | Encoder stuck for 70 ticks        |
| 4          | 0        | Encoder moving (no endstop)       |
| 6          | 0        | STOP sent, endstop handled        |

## How this differs from ivanwick

ivanwick's state machine has 5 states:
- `INPUT_IDLE`, `INPUT_UP`, `INPUT_DOWN`, `INPUT_MEM_UP`,
  `INPUT_MEM_DOWN`, `INPUT_SAVE` (6 actually, in the unified
  variant)

The OEM state machine has 6 effective states (0, 1, 2, 3, 4, 6)
plus default. The major differences are:

1. **State 1 → 2 factory-reset path** — ivanwick does not
   implement the 10-second both-buttons-hold factory reset.
2. **State 3 → 4 → 6 endstop detection** — ivanwick does not
   implement the endstop detector. This is the
   [issue #4](https://github.com/ivanwick/bekantfirmware/issues/4)
   fix.
3. **State 4 overshoot compensation** — ivanwick's `decel_counter`
   is a single counter, but the OEM uses a 16-bit overshoot
   value computed at 0x063C-0x0671.

The ivanwick-style `btn/btn.c` (5 states) is functionally a
**subset** of the OEM state machine. The ivanwick-style code
will work for basic UP/DOWN/MEM_UP/MEM_DOWN/SAVE operations
but will not perform factory-reset and will not detect the
endstop.
