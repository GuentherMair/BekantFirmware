# State-machine traces for the unified gesture handler

The state machine in `btn/btn.c` accepts **either** of the two
memory-position gestures and produces the same `INPUT_MEM_UP` /
`INPUT_MEM_DOWN` event. This file traces representative gestures so
you can audit the state transitions by hand.

Notation: `U` = UP pressed, `D` = DOWN pressed, `-` = released.
Multiple chars in one cell mean a single 50 ms debounced tick.

## State machine (high-level)

```
                  (U)                    (D)
                  ↓                      ↓
                [UP]   ← direction ←  [DOWN]
                  ↓                      ↓
       (both released)         (both released)
                  ↓                      ↓
   arm double-tap            arm double-tap
   window for U              window for D
                  ↓                      ↓
               [IDLE]  ←──── arm expired ──── [IDLE]
                  ↓                      ↓
       (U within 500 ms)       (D within 500 ms)
                  ↓                      ↓
   [MEM_UP] double-tap fired  [MEM_DOWN] double-tap fired
```

Additionally:

- `UP` and `DOWN` both pressed **while in `UP`** → `MEM_UP` (combo-press).
- `UP` and `DOWN` both pressed **while in `DOWN`** → `MEM_DOWN` (combo-press).
- Both buttons held for 3 s → `SAVE`.

## Trace A — tap `△` then release, no second tap (regular up movement)

```
gesture :   U   U   -   -   -   -   -   -   -
state   : IDLE UP  UP  IDLE   |  (timer ticks, no second tap)
events  :       INPUT_UP published
                                          IDLE (no second tap, no event)
```

Latches `waiting_for_second_up = true` and the 10-tick (500 ms)
window opens, but no second `U` arrives in time, so nothing else
fires.

## Trace B — combo-press: hold `△`, tap `▽` (memory-up, ivanwick style)

```
gesture :   U   U   U   D   U   -   -
state   : IDLE UP  UP  MEM_UP MEM_UP IDLE
events  :             ↑ INPUT_MEM_UP published (combo-press)
```

While in `UP`, the press of `D` (with `U` still held) jumps straight
to `MEM_UP`. The double-tap window is never armed in this path.

## Trace C — double-tap `△` within 500 ms (memory-up, DieSteinhose style)

```
gesture :   U   U   -   -   U   U   -   -
state   : IDLE UP  IDLE     UP  MEM_UP IDLE
events  :       INPUT_UP    ↑ INPUT_MEM_UP published (double-tap)
                 ↑ arm window, latch waiting_for_second_up = true
                                 ↑ second tap fires MEM_UP
```

On the first `U` release, the state machine returns to `IDLE` and
arms `waiting_for_second_up` with a 10-tick (500 ms) window. A second
`U` press within the window fires `INPUT_MEM_UP`.

## Trace D — direction change while moving (up → down without stopping)

```
gesture :   U   U   U   D   D   D
state   : IDLE UP  UP  DOWN DOWN DOWN
events  :       INPUT_UP    INPUT_DOWN
```

While in `UP`, a press of `D` (with `U` still held) transitions to
`DOWN`. The double-tap window is **not** armed because we did not go
through the "both released" transition.

## Trace E — single `▽` press (regular down movement)

```
gesture :   D   D   -   -   -   -   -
state   : IDLE DOWN DOWN IDLE  |  (timer ticks, no second tap)
events  :       INPUT_DOWN
```

Mirror of Trace A. The double-tap window is armed for `D` and
expires without firing.

## Trace F — combo-press for memory-down: hold `▽`, tap `△`

```
gesture :   D   D   D   U   D   -   -
state   : IDLE DOWN DOWN MEM_DOWN MEM_DOWN IDLE
events  :                ↑ INPUT_MEM_DOWN published
```

Mirror of Trace B. `DOWN` + `U` while in `DOWN` fires `INPUT_MEM_DOWN`.

## Trace G — double-tap `▽` within 500 ms (memory-down, DieSteinhose style)

```
gesture :   D   D   -   -   D   D   -   -
state   : IDLE DOWN IDLE     DOWN MEM_DOWN IDLE
events  :       INPUT_DOWN   ↑ INPUT_MEM_DOWN published
                 ↑ arm window
```

Mirror of Trace C.

## Trace H — both buttons held 3 s (save)

```
gesture :   U+D U+D U+D U+D U+D U+D U+D U+D U+D  ... (60 ticks)
state   : IDLE IDLE IDLE  ...                  SAVE
events  :                                ↑ INPUT_SAVE published
```

`save_hold` increments on every 50 ms tick while both are pressed;
when it reaches 60 (3 s), the state fires `INPUT_SAVE` and stays
in `INPUT_SAVE` until both buttons are released. `bui_save_pos()`
writes the new value to EEPROM and clicks the legs (BCMD_CLICK).

## Trace I — combo-press then quick re-tap (no spurious second memory)

```
gesture :   U   U   U   D   -   -   U   U   -   -
state   : IDLE UP  UP  MEM_UP IDLE     UP  IDLE
events  :             ↑ INPUT_MEM_UP     INPUT_UP (NOT a memory move)
```

After the first `INPUT_MEM_UP` fires, releasing both buttons returns
to `IDLE` *without* re-arming `waiting_for_second_up` (the latch is
only set in the `INPUT_UP` → release transition, not after a memory
fire). A quick re-tap of `U` is therefore a normal `INPUT_UP`, not
a spurious second memory move.

This is the edge case that justifies the unified state machine: a
pure DieSteinhose state machine would have armed the double-tap
window during the first `UP` and then fired a *second* `INPUT_MEM_UP`
when the user happened to press `U` again within 500 ms. Our state
machine is smart enough to suppress that.

## Trace J — interrupt a memory move (cancel by pressing the other button)

```
gesture :   U   U   U   D   U   D   -   -
state   : IDLE UP  UP  MEM_UP MEM_UP DOWN IDLE
events  :             ↑ INPUT_MEM_UP     INPUT_DOWN
                                          ↑ user pressed D, switch direction
```

From `MEM_UP`, pressing `D` (with `U` released or held) transitions
to `DOWN`. This lets the user cancel the memory-up and start moving
down immediately, matching the ivanwick behaviour.

## Re-running the traces

The state machine is exercised by `btn_gesture()` once per 50 ms
debounced tick. To sanity-check the traces, hand-simulate a few
gestures by feeding `(UP, DOWN)` boolean pairs into the `switch`
statement in `btn/btn.c` and walking the state transitions. The
"InputState_t" struct is fully observable from `btn_gesture()`'s
return value plus its static state.
