# Upstream comparison: ivanwick vs DieSteinhose

The two repos share the entire layout. The only C/H files that differ
are `btn/btn.c` and `btn/btn.h`. The README is also different
(DieSteinhose's README adds a "FORK" preamble that documents the
change). The `bekant/`, `lin/`, `main.c`, `interrupts.c`, `system.c`,
`user.c`, and `configuration_bits.c` files are byte-identical.

## File-level diff

```
$ diff -r --brief ivanwick-original/bekantfirmware.X \
                    diesteinhose-fork/bekantfirmware.X
Files .../btn/btn.c differ
Files .../btn/btn.h differ
```

(plus the README and the new `desk_diagram_double.svg` in the fork)

## btn.h diff

```diff
 typedef enum {
     INPUT_IDLE,
     INPUT_UP,
     INPUT_DOWN,
-    INPUT_MEM_UP,
-    INPUT_MEM_DOWN,
+    INPUT_DOUBLE_UP,    // New state for double-click up
+    INPUT_DOUBLE_DOWN,  // New state for double-click down
     INPUT_SAVE,
 } INPUT_t;
```

The fork **renames** the enumerators. The ivanwick code is consumed by
`bekant/bui.c` via these names — see `bui.c` for `INPUT_MEM_UP` /
`INPUT_MEM_DOWN`.

## btn.c state machine diff

### ivanwick (combo-press)

- From `INPUT_UP`:  if both UP+DOWN pressed → `INPUT_MEM_UP`
- From `INPUT_DOWN`: if both UP+DOWN pressed → `INPUT_MEM_DOWN`
- The MEM_UP/MEM_DOWN states stick until both buttons are released.

```c
case INPUT_UP:
    if      (UP_PRESSED && DOWN_RELEASED) input.state = INPUT_UP;
    else if (UP_RELEASED && DOWN_PRESSED) input.state = INPUT_DOWN;
    else if (UP_PRESSED && DOWN_PRESSED)  input.state = INPUT_MEM_UP;
    else                                  input.state = INPUT_IDLE;
    break;

case INPUT_DOWN:
    if      (UP_PRESSED && DOWN_RELEASED) input.state = INPUT_UP;
    else if (UP_RELEASED && DOWN_PRESSED) input.state = INPUT_DOWN;
    else if (UP_PRESSED && DOWN_PRESSED)  input.state = INPUT_MEM_DOWN;
    else                                  input.state = INPUT_IDLE;
    break;
```

### DieSteinhose (double-tap)

- From `INPUT_UP`:   when UP released → latch `waiting_for_second_up` for
  `DOUBLE_CLICK_WINDOW` (10 ticks = 500 ms) and return to IDLE.
  If a second UP press arrives inside the window → `INPUT_DOUBLE_UP`.
- Mirror for `INPUT_DOWN` / `INPUT_DOUBLE_DOWN`.

```c
uint8_t double_click_timer;
bool    waiting_for_second_up;
bool    waiting_for_second_down;

case INPUT_IDLE:
    if (UP_PRESSED && DOWN_RELEASED) {
        if (waiting_for_second_up) {
            waiting_for_second_up = false;
            state = INPUT_DOUBLE_UP;
        } else {
            state = INPUT_UP;
        }
    }
    // (symmetric for DOWN)
```

## What this project does differently

This project supports **both gestures at the same time**. There is no
compile-time switch. The unified state machine in `btn/btn.c` keeps
the combo-press transition (UP + DOWN while UP is held → `MEM_UP`)
**and** the double-tap window (UP released then re-pressed within
500 ms → `MEM_UP`) and fires the same `INPUT_MEM_UP` event from
either path. The same applies to `MEM_DOWN`.

User-facing consequence: someone who learned the combo-press from the
ivanwick fork can keep using it; someone who learned the double-tap
from the DieSteinhose fork can keep using that too. A new user can
pick whichever feels natural. The downstream BUI module does not (and
should not) know which one was used.

The state machine that achieves this is documented in
[`docs/TRACE.md`](TRACE.md).

## Consumer (bui.c) impact

`bekant/bui.c` continues to use `INPUT_MEM_UP` / `INPUT_MEM_DOWN`
unchanged from the ivanwick project. The ivanwick canonical names
are kept; the DieSteinhose aliases (`INPUT_DOUBLE_UP /
INPUT_DOUBLE_DOWN`) are not needed.

The ivanwick project's `bui.c` was already the reference for this
project; we did not need to vendor DieSteinhose's `bui.c`.

## Why two variants exist at all

- **ivanwick's combo-press** has zero false positives from accidental
  double-bumps (the user has to deliberately press-and-hold one button
  and tap the other), but it requires two hands and is unintuitive
  for new users.
- **DieSteinhose's double-tap** is the gesture people learn from
  elevator buttons and TV remotes; it's one-handed and obvious. The
  500 ms window is short enough that an intentional press never misses
  it but long enough that a user doing a normal up-down for fine
  adjustment won't trigger it.

Running both in parallel is strictly better than picking one:
- Existing users keep their muscle memory.
- New users get whichever feels natural.
- The cost is ~30 extra lines of C and 3 bytes in `.bss` — negligible
  on a PIC16LF1938 with 1 KB of RAM.
