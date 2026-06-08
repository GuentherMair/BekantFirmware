/**
 * @file btn.c
 * @brief BEKANT controller — recovered button state machine.
 *
 * This is a *simplified* reconstruction of the OEM firmware's button
 * handling. The full OEM state machine at 0x05c0 has 8 states and
 * handles a 10-second "press both buttons to factory-reset" sequence
 * (see 0x05d8: if 0x4c == 0x55 the state transitions to 0x02).
 *
 * The simplified state machine here captures the visible gestures
 * (UP / DOWN / MEM_UP / MEM_DOWN / SAVE) and delegates the factory-
 * reset sequence to bui.c.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include "btn.h"
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

/* Same PORTB mapping as ivanwick:
 *   RB0 = DOWN, RB1 = UP, RB2..RB7 = unused. */
typedef union {
    struct {
        unsigned DOWN : 1;
        unsigned UP   : 1;
        unsigned      : 6;
    };
    uint8_t value;
} ButtonState_t;

#define PRESSED(b)   (!(b))
#define RELEASED(b)  (b)
#define BUTTON_CHANGE(a, b)  ((a.UP != b.UP) || (a.DOWN != b.DOWN))

/* OEM firmware polls buttons at 4 kHz, debounces over 50 ms (= 200 ticks).
 * This matches ivanwick's timing exactly. */
#define DEBOUNCE_THRESHOLD  200

static bool btn_debounce(ButtonState_t now_btn) {
    static struct {
        uint8_t        count;
        ButtonState_t  last_state;
    } d = { 0, { 0 } };

    if (BUTTON_CHANGE(d.last_state, now_btn)) {
        d.count      = 0;
        d.last_state = now_btn;
    } else {
        d.count++;
    }

    if (d.count >= DEBOUNCE_THRESHOLD) {
        d.count = 0;
        return true;
    }
    return false;
}

/* 3 seconds of "both pressed" → save gesture.
 * 3 s * 20 Hz = 60 ticks. */
#define SAVE_HOLD_THRESHOLD  60

void (*btn_report_gesture)(INPUT_t gesture);

void btn_timer(void) {
    static INPUT_t last_input = INPUT_IDLE;
    static struct {
        uint8_t  save_hold;
        INPUT_t  state;
    } s = { 0, INPUT_IDLE };

    ButtonState_t bs;
    bs.value = PORTB;

    if (!btn_debounce(bs)) {
        return;
    }

    /* The full OEM state machine has 8 states (see disassembly 0x05c0).
     * Here we use the simpler ivanwick-style 6-state machine — the
     * OEM's two extra states are for the 10-second factory-reset
     * sequence, which is handled at a higher level. */
    switch (s.state) {
        case INPUT_IDLE:
            if      (PRESSED(bs.UP)   && RELEASED(bs.DOWN)) s.state = INPUT_UP;
            else if (RELEASED(bs.UP)  && PRESSED(bs.DOWN))  s.state = INPUT_DOWN;
            else if (PRESSED(bs.UP)   && PRESSED(bs.DOWN)) {
                if (++s.save_hold >= SAVE_HOLD_THRESHOLD) {
                    s.save_hold = 0;
                    s.state     = INPUT_SAVE;
                } else {
                    s.state = INPUT_IDLE;
                }
            }
            break;

        case INPUT_SAVE:
            if (PRESSED(bs.UP) && PRESSED(bs.DOWN)) s.state = INPUT_SAVE;
            else                                     s.state = INPUT_IDLE;
            break;

        case INPUT_UP:
            if      (PRESSED(bs.UP)   && RELEASED(bs.DOWN))  s.state = INPUT_UP;
            else if (RELEASED(bs.UP)  && PRESSED(bs.DOWN))   s.state = INPUT_DOWN;
            else if (PRESSED(bs.UP)   && PRESSED(bs.DOWN))   s.state = INPUT_MEM_UP;
            else                                              s.state = INPUT_IDLE;
            break;

        case INPUT_DOWN:
            if      (PRESSED(bs.UP)   && RELEASED(bs.DOWN))  s.state = INPUT_UP;
            else if (RELEASED(bs.UP)  && PRESSED(bs.DOWN))   s.state = INPUT_DOWN;
            else if (PRESSED(bs.UP)   && PRESSED(bs.DOWN))   s.state = INPUT_MEM_DOWN;
            else                                              s.state = INPUT_IDLE;
            break;

        case INPUT_MEM_UP:
            if      (PRESSED(bs.UP)   && RELEASED(bs.DOWN))  s.state = INPUT_MEM_UP;
            else if (RELEASED(bs.UP)  && PRESSED(bs.DOWN))   s.state = INPUT_DOWN;
            else if (PRESSED(bs.UP)   && PRESSED(bs.DOWN))   s.state = INPUT_MEM_UP;
            else                                              s.state = INPUT_IDLE;
            break;

        case INPUT_MEM_DOWN:
            if      (PRESSED(bs.UP)   && RELEASED(bs.DOWN))  s.state = INPUT_UP;
            else if (RELEASED(bs.UP)  && PRESSED(bs.DOWN))   s.state = INPUT_MEM_DOWN;
            else if (PRESSED(bs.UP)   && PRESSED(bs.DOWN))   s.state = INPUT_MEM_DOWN;
            else                                              s.state = INPUT_IDLE;
            break;
    }

    if (s.state != last_input) {
        last_input = s.state;
        btn_report_gesture(s.state);
    }
}

void btn_init(void) {
    /* OEM firmware uses the same Timer2 settings as ivanwick:
     *   Fosc = 16 MHz, F_cy = 4 MHz
     *   prescaler 1:1, period 100, postscaler 1:10 → 4 kHz interrupt
     * Each debounce tick is 250 µs; 200 ticks = 50 ms. */
    T2CONbits.T2CKPS  = 0b00;       // prescaler 1:1
    PR2 = 100;                       // period
    T2CONbits.T2OUTPS = 0b1001;     // postscaler 1:10
    T2CONbits.TMR2ON  = 1;
    PIE1bits.TMR2IE   = 1;
}
