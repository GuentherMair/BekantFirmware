/*
 * btn.c — Button gesture state machine (unified: combo-press + double-tap).
 *
 * Either of the two encodings fires the same INPUT_MEM_UP / INPUT_MEM_DOWN
 * event.  See btn.h for the high-level description and USAGE.md for the
 * end-user docs.
 *
 * State-machine summary
 * ---------------------
 *   IDLE    --(U)-->    UP
 *   IDLE    --(D)-->    DOWN
 *   IDLE    --(U,U) within 500 ms-->  MEM_UP   (double-tap)
 *   IDLE    --(D,D) within 500 ms-->  MEM_DOWN (double-tap)
 *   UP      --(U + D)-->             MEM_UP   (combo-press: hold U, tap D)
 *   DOWN    --(D + U)-->             MEM_DOWN (combo-press: hold D, tap U)
 *   UP      --(both released)-->     IDLE + arm double-tap window for U
 *   DOWN    --(both released)-->     IDLE + arm double-tap window for D
 *   UP      --(D only)-->            DOWN     (direction change)
 *   DOWN    --(U only)-->            UP       (direction change)
 *   MEM_*   --(any input)-->         same or IDLE on release
 *   IDLE    --(U+D) for 3 s-->       SAVE
 */

#include "btn.h"
#include <xc.h>
#include <stdbool.h>       /* For true/false definition */
#include <stdint.h>        /* For uint8_t definition */

/* A mapping to PORTB — determined by PCB traces.
 * RB0 = DOWN, RB1 = UP, RB2..RB7 unused. */
typedef union {
    struct {
        unsigned DOWN : 1;
        unsigned UP   : 1;
        unsigned      : 6;
    };
} ButtonState_t;

#define PRESSED(b)    (!(b))
#define RELEASED(b)   (b)
#define BUTTON_CHANGE(a, b)  ((a.UP != b.UP) || (a.DOWN != b.DOWN))

typedef struct {
    uint8_t        count;
    ButtonState_t  last_state;
} Debounce_t;

/* Polled at Timer2 frequency: 4000 Hz, so every 250 µs.
 * 250 µs × 200 ticks = 50 ms. */
#define DEBOUNCE_THRESHOLD  200

static bool btn_debounce(ButtonState_t now_btn) {
    static Debounce_t d = { 0, { 0, 0 } };

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

/* State that the debounce loop calls once per 50 ms tick. */
typedef struct {
    uint8_t  save_hold;
    /* Double-tap tracking — always on. */
    uint8_t  double_click_timer;
    bool     waiting_for_second_up;
    bool     waiting_for_second_down;
    INPUT_t  state;
} InputState_t;

/* SAVE_HOLD_THRESHOLD: 50 ms × 60 = 3 seconds of both-buttons-held. */
#define SAVE_HOLD_THRESHOLD  60

/* Double-click window: 50 ms × 10 = 500 ms. */
#define DOUBLE_CLICK_WINDOW  10

INPUT_t btn_gesture(ButtonState_t btn) {
    static InputState_t input = {
        .save_hold              = 0,
        .double_click_timer     = 0,
        .waiting_for_second_up  = false,
        .waiting_for_second_down = false,
        .state                  = INPUT_IDLE,
    };

    /* Tick the double-click window.  When it expires the "waiting for
     * second tap" latches are released. */
    if (input.double_click_timer > 0) {
        input.double_click_timer--;
        if (input.double_click_timer == 0) {
            input.waiting_for_second_up   = false;
            input.waiting_for_second_down = false;
        }
    }

    switch (input.state) {
        case INPUT_IDLE:
            if (PRESSED(btn.UP) && RELEASED(btn.DOWN)) {
                if (input.waiting_for_second_up) {
                    /* Double-tap UP detected. */
                    input.waiting_for_second_up = false;
                    input.double_click_timer   = 0;
                    input.state                = INPUT_MEM_UP;
                } else {
                    input.state = INPUT_UP;
                }
            } else if (RELEASED(btn.UP) && PRESSED(btn.DOWN)) {
                if (input.waiting_for_second_down) {
                    input.waiting_for_second_down = false;
                    input.double_click_timer     = 0;
                    input.state                  = INPUT_MEM_DOWN;
                } else {
                    input.state = INPUT_DOWN;
                }
            } else if (PRESSED(btn.UP) && PRESSED(btn.DOWN)) {
                if (++input.save_hold >= SAVE_HOLD_THRESHOLD) {
                    input.save_hold = 0;
                    input.state     = INPUT_SAVE;
                } else {
                    input.state = INPUT_IDLE;
                }
            } else { // both RELEASED
                input.state = INPUT_IDLE;
            }
            break;

        case INPUT_SAVE:
            input.state = (PRESSED(btn.UP) && PRESSED(btn.DOWN))
                            ? INPUT_SAVE : INPUT_IDLE;
            break;

        case INPUT_UP:
            if (PRESSED(btn.UP) && PRESSED(btn.DOWN)) {
                /* Combo-press: UP is held, DOWN was tapped.  Fire memory. */
                input.state = INPUT_MEM_UP;
            } else if (RELEASED(btn.UP) && PRESSED(btn.DOWN)) {
                input.state = INPUT_DOWN;
            } else if (PRESSED(btn.UP) && RELEASED(btn.DOWN)) {
                input.state = INPUT_UP;
            } else { // both RELEASED — start double-tap window for UP
                input.waiting_for_second_up = true;
                input.double_click_timer   = DOUBLE_CLICK_WINDOW;
                input.state                = INPUT_IDLE;
            }
            break;

        case INPUT_DOWN:
            if (RELEASED(btn.UP) && PRESSED(btn.DOWN)) {
                input.state = INPUT_DOWN;
            } else if (PRESSED(btn.UP) && PRESSED(btn.DOWN)) {
                /* Combo-press: DOWN is held, UP was tapped. */
                input.state = INPUT_MEM_DOWN;
            } else if (PRESSED(btn.UP) && RELEASED(btn.DOWN)) {
                input.state = INPUT_UP;
            } else { // both RELEASED
                input.waiting_for_second_down = true;
                input.double_click_timer     = DOUBLE_CLICK_WINDOW;
                input.state                  = INPUT_IDLE;
            }
            break;

        case INPUT_MEM_UP:
            if (PRESSED(btn.UP) && RELEASED(btn.DOWN)) {
                input.state = INPUT_MEM_UP;
            } else if (RELEASED(btn.UP) && PRESSED(btn.DOWN)) {
                /* User changed their mind mid-memory-move. */
                input.state = INPUT_DOWN;
            } else if (PRESSED(btn.UP) && PRESSED(btn.DOWN)) {
                input.state = INPUT_MEM_UP;
            } else { // both RELEASED
                input.state = INPUT_IDLE;
            }
            break;

        case INPUT_MEM_DOWN:
            if (RELEASED(btn.UP) && PRESSED(btn.DOWN)) {
                input.state = INPUT_MEM_DOWN;
            } else if (PRESSED(btn.UP) && RELEASED(btn.DOWN)) {
                input.state = INPUT_UP;
            } else if (PRESSED(btn.UP) && PRESSED(btn.DOWN)) {
                input.state = INPUT_MEM_DOWN;
            } else { // both RELEASED
                input.state = INPUT_IDLE;
            }
            break;
    }

    return input.state;
}

void (*btn_report_gesture)(INPUT_t gesture);

void btn_timer(void) {
    static INPUT_t last_input = INPUT_IDLE;

    ButtonState_t button_state;
    button_state.UP   = PORTBbits.UP;
    button_state.DOWN = PORTBbits.DOWN;

    if (btn_debounce(button_state)) {
        INPUT_t input = btn_gesture(button_state);

        if (input != last_input) {
            last_input = input;
            btn_report_gesture(input);
        }
    }
}

void btn_init(void) {
    /* Timer2 clock input is Fosc/4 (instruction clock).
     * System Fosc: 16 MHz → F_cy = 4 MHz.
     * 4 MHz / 1 (prescaler) / 100 (period) / 10 (postscaler) = 4000 Hz,
     * i.e. 250 µs per tick. */
    T2CONbits.T2CKPS  = 0b00;     // prescaler 1:1
    PR2bits.PR2       = 100;      // period
    T2CONbits.T2OUTPS = 0b1001;   // postscaler 1:10
    T2CONbits.TMR2ON  = 1;        // timer on
    PIE1bits.TMR2IE   = 1;        // enable Timer2 interrupt
}
