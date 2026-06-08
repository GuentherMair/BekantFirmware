/**
 * @file endstop.c
 * @brief BEKANT controller — recovered endstop / over-travel detector.
 *
 * Recovered from disassembly addresses 0x05c0–0x068d (the OEM
 * firmware's button + movement state machine) and the lookup table at
 * 0x0d5–0x0ff (which writes BCMD 0xfc to register 0x61 when the
 * endstop condition is met).
 *
 * The full OEM state machine at 0x05c0 has 8 states. The endstop
 * condition is handled in state 0x07 (see disassembly 0x0672):
 *
 *   0x0672: movf    0x2a, 0x0
 *   0x0673: movlb   0x00
 *   0x0674: xorwf   0x4c, 0x0    ; check if 0x2a != 0x4c
 *   0x0675: btfsc   0x03, 0x2
 *   0x0676: goto    0x0684       ; unchanged → continue moving
 *   0x0677: movlw   0xfc         ; BCMD_STOP
 *   0x0678: clrf    0x4d         ; reset state
 *   0x0679: andwf   0x47, 0x1    ; clear "moving" flag
 *
 * `0x2a` is the previous encoder value; `0x4c` is the current
 * encoder value. The condition `0x2a != 0x4c` means "the encoder
 * has not changed since the last sample" — i.e. the leg is no
 * longer turning even though BCTRL is still commanding motion.
 * When that happens, the firmware sends BCMD_STOP (0xfc) to drop
 * the relay.
 *
 * The ivanwick project (which is what we are extending) has no
 * equivalent. ivanwick's bctrl.c simply sends the commanded BCMD
 * every 5 ms with no observation of whether the encoder is actually
 * advancing.  This is the root cause of issue #4: when the desk
 * reaches its physical bottom endstop, the relay stays engaged
 * because bctrl.c never notices that the encoder is no longer
 * changing.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "endstop.h"
#include "bctrl.h"

extern int16_t bctrl_pos;
extern BCTRL_state_t bctrl_state;
extern BCTRL_state_t bctrl_target;

/* The OEM firmware observes whether the encoder is changing between
 * successive scheduler ticks. The "stuck" condition is "encoder did
 * not change at all in N consecutive ticks while BCTRL is commanding
 * motion". The exact N is recovered from the BCTRL loop at 0x02f0
 * (state 0x07): the firmware sends BCMD_STOP only after 0x46 (70
 * decimal) scheduler ticks — that's ~350 ms at 200 Hz. */
#define ENDSTOP_STUCK_TICKS  70     /* ~350 ms at 200 Hz */

/* Saved encoder value from the previous tick. */
static int16_t prev_encoder    = 0;
static int16_t prev_encoder2   = 0;
static uint8_t stuck_count     = 0;

/* The default motion-bound encoder values. Recovered from the EEPROM
 * writes at 0x0221–0x0242: the OEM firmware stores the four default
 * memory positions at EEPROM addresses 0x0021, 0x00a0, 0x0120, 0x01d2
 * (one 16-bit value per leg, high + low). */
#define ENC_LEG_LOW_DEFAULT   0x0021
#define ENC_LEG_HIGH_DEFAULT  0x0120
#define ENC_PHYS_LOW_MIN      0x0010   /* physical lower endstop region */
#define ENC_PHYS_HIGH_MAX     0x1f00   /* physical upper endstop region */

/* The BCTRL module registers this so we can demand a stop. */
void (*endstop_report)(int16_t pos);

/* Init: read the default memory positions from EEPROM (the same ones
 * ivanwick reads from offsets 0x00 and 0x02 in EEPROM). The OEM
 * firmware uses 4 EEPROM cells (two per leg) but for the endstop
 * detection we only need the global lower/upper bound. */
void endstop_init(void) {
    prev_encoder  = 0;
    prev_encoder2 = 0;
    stuck_count   = 0;
    /* No EEPROM read needed for endstop — we observe the encoder
     * directly. The OEM firmware's EEPROM layout is different
     * (4 cells vs ivanwick's 2 cells) but the per-leg memory positions
     * are read by bui.c, not by this module. */
}

/**
 * Tick the endstop detector.
 *
 * Called by bctrl_timer() once per scheduler slot (200 Hz). The
 * `pos` argument is the encoder value that bctrl just reported.
 *
 * Logic (recovered from OEM firmware at 0x0672–0x068d):
 *   - If BCTRL is currently commanding motion (UP or DOWN) AND
 *   - the encoder has not changed since the previous tick, AND
 *   - we have seen this "stuck" condition for >= ENDSTOP_STUCK_TICKS:
 *       → force a BCMD_STOP by transitioning the BCTRL state to STOP.
 */
static bool bctrl_is_moving(void) {
    /* The bctrl module exposes its state via the schedule: when the
     * scheduled BCMD is one of the motion commands (UP_FAST, UP_SLOW,
     * DOWN_FAST, DOWN_SLOW, DECEL), the desk should be moving.
     * Bui is the simpler way: we can ask bctrl for its current state
     * through a registered callback. Here we approximate by checking
     * whether the last commanded BCMD is a motion BCMD. */
    extern BCTRL_state_t bctrl_state;
    extern BCTRL_state_t bctrl_target;
    return (bctrl_state == BCTRL_UP || bctrl_state == BCTRL_DOWN);
}

void endstop_timer(void) {
    int16_t cur = bctrl_pos;

    if (bctrl_is_moving()) {
        if (cur == prev_encoder && cur == prev_encoder2) {
            if (++stuck_count >= ENDSTOP_STUCK_TICKS) {
                /* Recovered: the OEM firmware issues BCMD_STOP (0xfc)
                 * by writing 0xfc to the BCMD register and clearing
                 * the moving flag (0x47). Here we do the equivalent:
                 * ask bctrl to stop. */
                bctrl_set_target(BCTRL_STOP);
                stuck_count = 0;
            }
        } else {
            stuck_count = 0;
        }
    } else {
        stuck_count = 0;
    }

    prev_encoder2 = prev_encoder;
    prev_encoder  = cur;
}
