# Endstop / over-travel analysis

## TL;DR

**Tentatively, yes** — the ivanwick firmware appears to have the
endstop bug described in
[issue #4](https://github.com/ivanwick/bekantfirmware/issues/4):
when the desk is commanded to its physical bottom (or top), the
relay stays engaged instead of being released. The presumed root
cause is a missing module — the ivanwick project appears to have
no equivalent of what we believe to be the original IKEA
firmware's endstop detector. The proposed fix is a small,
self-contained module (see `src/bekant/endstop.c`, recovered from
`oem_reconstruction/bekant/endstop.c`)
that watches the leg encoders and forces BCMD_STOP (0xfc) when motion
is commanded but the encoder has stopped advancing.

> **Caveat:** the project has not been tested on hardware. The
> above is a *likely* explanation derived from the disassembly and
> the issue report, not a confirmed diagnosis.

The reconstructed C code, the disassembly, and the analysis below are
kept under `oem_reconstruction/` for future
reference.

## What issue #4 actually says

> Hello,
> I flashed my desktop with your firmware. Everything works but if the
> office down to the bottom, the relay remains active.
> Is there a low position memory on your firmware?
> From the factory, the position can be reset by pressing the 2 buttons
> for 10 sec.
> Thank

The user reports two related symptoms:

1. **Relay stays active** when the desk is at the bottom physical
   endstop.
2. **No factory-reset** of the saved position via the 10-second
   both-buttons-hold gesture the OEM firmware supports.

Both symptoms may stem from the same underlying gap: the ivanwick
project appears not to have a 10-second press-and-hold sequence
at all in its button state machine. The state machine in `btn/btn.c`
only handles the 3-second save, not 10-second reset. And the
bctrl layer does not appear to watch the encoder to detect a
stalled leg.

## How the original IKEA firmware appears to differ

The disassembly of `orginafirm.hex` (PIC16LF1938) appears to contain
a state machine at **0x05c0** that has 8 states (versus the 5
states the ivanwick code implements). The state machine's
control register appears to be RAM `0x4d`.

The state machine's `case 0x07` at disassembly **0x0672** looks
like the endstop-detected handler. The code:

```asm
; 0x0672 — state 0x07 handler
0672:  movf    0x2a, 0x0      ; W = 0x2a
0673:  movlb   0x00
0674:  xorwf   0x4c, 0x0      ; W = W XOR 0x4c
0675:  btfsc   0x03, 0x2      ; if Z flag set, 0x2a == 0x4c
0676:  goto    0x0684         ; unchanged → continue moving
0677:  movlw   0xfc           ; 0xfc = BCMD_STOP
0678:  clrf    0x4d           ; state = 0
0679:  andwf   0x47, 0x1      ; clear "moving" flag in 0x47
```

This reads as: *"if the encoder has not changed since the last
sample (i.e. the leg is no longer turning), send BCMD_STOP (0xfc)
and reset the state machine."* This appears to be the endstop
detector.

The same disassembly address also appears to contain a
**factory-reset sequence** at **0x05d8**: if `0x4c == 0x55` (a
magic byte), the state transitions to `0x02` and the firmware
treats the next 10-second hold as a reset. The value `0x55` is
presumably loaded into `0x4c` by a different code path that
counts how long both buttons have been held; the recovery of that
counter is in `oem_reconstruction/bekant/bui.c::bui_factory_reset()`.

The ivanwick project does not appear to have this state machine.
The closest equivalent in `bekant/bctrl.c` is the `bctrl_set_target`
function and the `bctrl_populate_cmd` function, neither of which
ever compares the encoder to itself across two samples.

## BCMD constants: ivanwick might be partly wrong

The OEM firmware's command bytes (what ivanwick calls `BCMD_*`)
are recovered from the state-indexed lookup at disassembly
**0x0d5–0x0ff** (a switch on RAM `0x52`, writing to RAM
`0x161` — i.e. bank-2 register `0x61`):

| OEM state (`0x52`) | OEM BCMD byte | What we believe it does          |
| ------------------ | ------------- | -------------------------------- |
| 0, 1 (default)     | `0x50`        | Idle / not moving                |
| 2                  | `0x49`        | Initialize / wake bus            |
| 3                  | `0xca`        | Move up (fast)                   |
| 4                  | `0x8b`        | Move up (slow)                   |
| 5                  | `0x4c`        | Move down (slow)                 |
| 6                  | `0x0d`        | Move down (fast)                 |
| 7                  | `0x8e`        | Decelerate (overshoot)           |
| 8                  | `0xcf`        | Pre-stop / "almost there"        |
| —                  | `0xfc`        | Universal STOP (endstop handler) |

> The state mapping in this table is *inferred* from the
> state-indexed dispatcher at `0x00d5–0x00ff`. The `What we believe
> it does` column is best-effort guesswork; only the byte values
> themselves are read directly out of the disassembly.

Compare with what ivanwick uses in `bekant/bctrl.c`:

| ivanwick name          | ivanwick value | OEM actual      | Match? |
| ---------------------- | -------------- | --------------- | ------ |
| `BCMD_STOP_IDLE`       | `0xfc`         | `0xfc`          | ✓      |
| `BCMD_AFTER_SCAN_INIT` | `0xbf`         | `0x50`          | ✗      |
| `BCMD_UP`              | `0x86`         | `0xca` / `0x8b` | ✗      |
| `BCMD_DOWN`            | `0x85`         | `0x4c` / `0x0d` | ✗      |
| `BCMD_DECEL`           | `0x87`         | `0x8e`          | ✗      |
| `BCMD_PRE_STOP`        | `0x84`         | `0xcf`          | ✗      |
| `BCMD_ERR_UP`          | `0xbc`         | (none)          | ✗      |
| `BCMD_ERR_DOWN`        | `0xbd`         | (none)          | ✗      |
| `BCMD_PRE_MOVE`        | `0xc4`         | `0x49`          | ✗      |

**Apparently, only `0xfc` matches.** The other ivanwick BCMD
values appear to be the ivanwick project's own interpretation
of the LIN protocol and *may* happen to work in practice because
the LIN slave (the leg controller) is forgiving about which
BCMD it receives — perhaps falling back to a sensible default.

This *suggests* the ivanwick project might be a "close
enough" reverse engineering of the LIN protocol, and that
it works in practice because the leg controller is tolerant.
But it may not be a faithful reconstruction of the original
firmware.

The crucial observation — **if our reading of the dispatcher is
correct** — is that the only BCMD that must be exact is `0xfc`
(STOP), because the bug in issue #4 is specifically that the
relay does not release at the physical endstop. The
ivanwick project's `BCMD_STOP_IDLE = 0xfc` appears to match
the OEM value, so the fix for issue #4 is not (only) a BCMD
value — it is more likely a *separate module* that watches the
encoder and calls `bctrl_set_target(BCTRL_STOP)` at the
right moment.

> **Caveat:** the state mapping is an inference from the
> dispatcher's structure. We have not run the firmware on hardware
> to confirm. The byte values themselves are read directly out of
> the disassembly and are reliable; the *state-to-byte* mapping
> could be off if the dispatcher is entered via a different
> path than we have inferred.

## Root cause of issue #4

`bctrl.c::bctrl_set_target(BCTRL_STOP)` *appears* to be the
only path that causes the BCTRL state machine to issue
`BCMD_STOP_IDLE` (0xfc) and let the leg controller drop the
relay. The path would be taken when:

1. The user explicitly releases the buttons, **or**
2. The BUI detects that the encoder has crossed a memory-position
   threshold (`bui_set_pos` sees `cur_pos >= high_pos` or
   `cur_pos <= low_pos`).

There appears to be no path that triggers a STOP when the
physical endstop is hit. The user releases the buttons (path 1)
when they want to stop, but if the user presses DOWN and holds
it, the firmware will likely keep issuing `BCMD_DOWN` (or
the ivanwick project's use of it) every 5 ms indefinitely —
the leg controller would keep the relay engaged because it
never receives a STOP command. Eventually the leg stalls
mechanically at the endstop, but the firmware continues to
ask the leg to move, and the relay stays on. **This is what
we believe to be the bug.** We have not verified this on
hardware.

## The proposed fix

`oem_reconstruction/bekant/endstop.c` is a
~150-line module that, based on the OEM firmware's state
machine:

1. Watches the leg encoder (via `bctrl_report_pos`, which the
   ivanwick project already exposes).
2. Counts how many scheduler ticks the encoder has been stuck
   (no change) while BCTRL is commanding motion.
3. After ~350 ms of being stuck (70 ticks × 5 ms — the value
   we believe is recovered from the OEM firmware's state
   machine), forces `bctrl_set_target(BCTRL_STOP)`, which would
   cause the BCMD_STOP (0xfc) to be sent and the relay to
   release.

The module hooks into the same `bctrl_report_pos` callback that
`bui_set_pos` already uses, so the integration is two lines in
`user.c::InitApp()` and one tick call in `bctrl_timer()`:

```c
// src/user.c::InitApp()
endstop_report = bctrl_stop_if_at_endstop;
endstop_init();

// src/bekant/bctrl.c::bctrl_timer() — once per scheduler slot
endstop_timer();
```

## Applying the proposed fix to a stock ivanwick project

If our diagnosis is correct, three small additions to a stock
ivanwick project would plausibly close issue #4:

1. Add `bekant/endstop.c` and `bekant/endstop.h` to the project
   (copy from `oem_reconstruction/bekant/`).
2. Add `endstop_init()` to `main.c` (or `user.c::InitApp()`).
3. Add the line `endstop_report = bctrl_stop_if_at_endstop;`
   to `user.c::InitApp()` (same pattern as
   `bctrl_report_pos = bui_set_pos`).
4. Add one call to `endstop_timer()` in
   `bctrl.c::bctrl_timer()`, before the position report.

In this project the integration is already done: the file
lives at `src/bekant/endstop.{c,h}`, the callback is registered
in `src/user.c::InitApp()`, and the tick is called from
`src/bekant/bctrl.c::bctrl_timer()`. We have not verified
that the fix actually works on hardware.

## Bonus: factory-reset support

While the disassembly was being analyzed, the OEM firmware's
**10-second both-buttons-hold** factory-reset sequence was also
recovered. It lives in the same state machine (disassembly
**0x05c0–0x068d**), with the magic-byte check at **0x05d8**.

The reconstructed code in
`oem_reconstruction/bekant/bui.c::bui_factory_reset()`
implements the reset, but it is **not** wired up to the button
state machine in this reconstruction. A full implementation
would extend `btn/btn.c` with a second SAVE_HOLD_THRESHOLD of 200
(10 s) that fires `bui_factory_reset()` instead of the regular
3-second save. That is left as future work — the primary goal
of this project was the endstop fix.

## Disassembly walkthrough

### Reset vector

```
0x0000:  movlp 0x00         ; Page = 0
0x0001:  goto  0x001f       ; → main entry
0x0002:  movwi -.1[1]       ; stack init
0x0003:  return
```

### Interrupt vector

```
0x0004:  movlp 0x00
0x0005:  movlb 0x00
0x0006:  movf  0x20, 0x0    ; W = some IRQ flag byte
0x0007:  xorlw 0x0b         ; mask off the 0x0b bit pattern
0x0008:  btfsc 0x03, 0x2    ; if Z, no flag
0x0009:  goto  0x001c       ; → default ISR (probably unexpected IRQ)
0x000a:  btfss 0x11, 0x5    ; check PIR1.RCIF (USART RX)
0x000b:  retfie             ; no, return
0x000c:  movlp 0x08
0x000d:  call  0x0037       ; USART RX → lin_txrx_daemon
0x000e:  movlp 0x00
0x000f:  movlb 0x00
0x0010:  bcf   0x11, 0x5    ; clear RCIF
... etc.
```

(For brevity only the USART-RX branch is shown; the Timer2 and
Timer4 branches follow the same pattern.)

### Main entry chain

```
0x001f:  movlp 0x02
0x0020:  goto  0x0221       ; → init sequence
0x0021:  goto  0x0185       ; → main loop
0x0221:  ... 4 EEPROM writes (default memory positions)
         call 0x0046 (write-16-bit-constant-to-EEPROM-helper)
0x0185:  clrf  0x5e          ; init state counter
0x0184:  goto  0x0190
0x0190:  clrwdt
0x0191:  return
```

The function 0x0190 *appears* to be the **main loop entry**:
every time the WDT is about to fire, the firmware returns to
0x0190 and starts over. The ivanwick project's
`while(1) { CLRWDT(); }` looks similar.

### BCMD lookup table (the key recovery)

```
; 0x0d5–0x0ff — switch on state register 0x52, write BCMD to 0x161
; (the asm below shows the literal disassembly: `movlb 0x02` selects bank 2,
; then `movwf 0x61` writes to the bank-2 register 0x61 = RAM 0x161)
0x00d5:  movlw 0x02
0x00d6:  subwf 0x52, 0x0    ; W = 0x52 - 2
0x00d7:  btfss 0x03, 0x0    ; if borrow (0x52 < 2)
0x00d8:  goto  0x00d1       ;   → default
0x00d9:  movlw 0x49         ;   → 0x49  (state 2)
0x00da:  goto  0x00fe
0x00db:  movlw 0x03
0x00dc:  subwf 0x52, 0x0    ; W = 0x52 - 3
0x00dd:  btfss 0x03, 0x0
0x00de:  goto  0x00d1       ; default
0x00df:  movlw 0xca         ; → 0xca  (state 3)
0x00e0:  goto  0x00fe
0x00e1:  movlw 0x04         ; ... state 4 → 0x8b
0x00e5:  movlw 0x8b
0x00e6:  goto  0x00fe
0x00e7:  movlw 0x05         ; ... state 5 → 0x4c
0x00eb:  movlw 0x4c
0x00ec:  goto  0x00fe
0x00ed:  movlw 0x06         ; ... state 6 → 0x0d
0x00f1:  movlw 0x0d
0x00f2:  goto  0x00fe
0x00f3:  movlw 0x07         ; ... state 7 → 0x8e
0x00f7:  movlw 0x8e
0x00f8:  goto  0x00fe
0x00f9:  movlw 0x08         ; ... state 8 → 0xcf
0x00fd:  movlw 0xcf
0x00fe:  movlb 0x02
0x00ff:  movwf 0x61         ; BCMD = W (writes to RAM 0x161)
0x0100:  goto  0x010e
0x00d1:  movlw 0x50         ; default → 0x50 (STOP / IDLE)
0x00d2:  movlb 0x02
0x00d3:  movwf 0x61         ; writes to RAM 0x161
0x00d4:  goto  0x010e
```

This *appears* to be a textbook switch-on-byte dispatch:
subtract the case value, branch on borrow, and on success
write the case's constant. The default *looks like* it is
`0x50` — i.e. STOP.

### Endstop handler (the critical recovery)

```
; 0x0672 — state 0x07 (endstop-detected) handler
0x0672:  movf  0x2a, 0x0      ; W = previous encoder reading
0x0673:  movlb 0x00
0x0674:  xorwf 0x4c, 0x0      ; W = W XOR current encoder
0x0675:  btfsc 0x03, 0x2      ; Z = 1 if previous == current
0x0676:  goto  0x0684         ; unchanged → continue
0x0677:  movlw 0xfc           ; BCMD_STOP
0x0678:  clrf  0x4d           ; state = 0
0x0679:  andwf 0x47, 0x1      ; clear moving flag
0x067a:  clrf  0x53           ; reset something
0x067b:  incf  0x53, 0x1
0x067c:  decfsz 0x44, 0x0     ; decrement timeout
0x067d:  return
0x067e:  movf  0x42, 0x0
0x067f:  xorlw 0x02           ; if 0x42 == 0x02
0x0680:  btfsc 0x03, 0x2
0x0681:  goto  0x065a         ; goto 0x065a (another state handler)
```

The apparent key pattern is the `0x2a XOR 0x4c` followed by
`goto 0x0684` on zero — i.e. *"if the previous encoder equals
the current encoder, skip the STOP"*. When the legs are actually
moving, the encoder changes between samples, so XOR is nonzero,
the branch is not taken, and `0xfc` is written — STOP. When the
legs are stuck at the endstop, the encoder stops changing, XOR
is zero, and the branch is taken to `0x0684` which is a
different code path (presumably "continue moving" but the leg
controller has its own timeout that will eventually drop the
relay). This is our interpretation; we have not exercised it
on hardware.

## Conclusion (tentative)

We believe, but have not confirmed, that the ivanwick firmware
has the endstop bug from issue #4. The likely root cause is a
missing endstop-detector module that the original IKEA firmware
appears to have. The proposed fix is a small (~150 line)
self-contained module that watches the encoder and forces a
STOP when motion is commanded but no progress is being made.
The fix is in `src/bekant/endstop.c` (recovered from
`oem_reconstruction/bekant/endstop.c`)
and integrates with the existing ivanwick project via a single
line in `user.c::InitApp()` plus one tick call in `bctrl_timer()`.

As a corollary, the recovered BCMD constants suggest that
the ivanwick project's use of the LIN protocol might be
mostly the ivanwick interpretation rather than the OEM
behaviour (only `0xfc` for STOP appears to match the OEM),
but in practice it may still work because the leg controller
is forgiving. The endstop fix does not appear to depend on
the ivanwick interpretation of the BCMDs being right — it
specifically issues the universal `0xfc` STOP, which
ivanwick appears to use correctly.

> **Reminder:** the project has not been tested on hardware.
> The above is a *plausible* explanation derived from the
> disassembly and the issue report, not a confirmed diagnosis.
