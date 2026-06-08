/**
 * @file bctrl.c
 * @brief BEKANT controller — recovered 8-state LIN/BCTRL control plane.
 *
 * This is a best-effort C reconstruction of the OEM firmware's
 * BCTRL state machine. The OEM firmware has a 6-state BCTRL
 * state machine (0, 1, 2, 3, 4, 6) plus default, with two
 * orthogonal dispatch tables:
 *
 *   - 0x1000-0x1043: a 34-entry dispatch (state 0-34) that
 *     routes to the BCMD-write functions (0x00cc, 0x00d5,
 *     0x00db, 0x00e1, 0x00e7, 0x00ed, 0x00f3, 0x00f9) for
 *     the 8 BCMD byte values, plus 0x00d4 (no-op) for
 *     intermediate states and 0x0298-0x02b7 for the
 *     post-move / endstop handlers.
 *
 *   - 0x1800-0x183f: a 32-entry dispatch (state 0-31) that
 *     routes to the LIN bus state handlers (0x0022, 0x0024,
 *     0x006c, 0x00c2, 0x0119, 0x0137, 0x03xx, 0x04xx, 0x05xx,
 *     0x06xx).
 *
 * The 0x05C0 dispatcher is the **BCTRL state machine** that
 * handles the high-level control flow:
 *
 *   state 0 (default)  → return / no-op
 *   state 1            → check factory-reset magic byte (0x55)
 *   state 2            → factory-reset armed (transition from state 1)
 *   state 3            → encoder pre-check (start sampling)
 *   state 4            → encoder endstop detection (sample-and-compare)
 *   state 5            → no-op (default)
 *   state 6            → ENDSTOP detected → send BCMD_STOP (0xfc)
 *   state 7, 8+        → no-op (default)
 *
 * The actual endstop detection logic is in the state 4 handler
 * at 0x060A-0x0671. The handler reads the encoder twice (at
 * 0x4c and a delayed value at 0x4c), compares them, and if
 * they are equal (the encoder has stopped advancing) it
 * transitions to state 6 which sends BCMD_STOP.
 *
 * Best-effort reconstruction — see ANALYSIS.md and
 * ../docs/ENDSTOP_ANALYSIS.md.
 */

#include "bctrl.h"
#include "bcmd_table.h"
#include "../lin/lin_d.h"
#include "endstop.h"
#include "eeprom_defaults.h"
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/* State register: 0x4d in OEM RAM. */
BCTRL_state_t bctrl_state = BCTRL_STATE_IDLE;

/* BCMD state register: 0x52 in OEM RAM. */
BCMD_state_t bcmd_state  = BCMD_STATE_DEFAULT;

/* Current encoder reading (signed 16-bit). */
int16_t bctrl_encoder;

/* Current position (for endstop.c and others). */
int16_t bctrl_pos;

/* Target state (for endstop.c and others). */
uint8_t bctrl_target;

/* Previous encoder reading, used for endstop detection. */
int16_t bctrl_encoder_prev;

/* Stall counter: incremented each scheduler tick while the
 * encoder is stuck. Cleared when the encoder starts moving. */
uint8_t bctrl_stall_counter = 0;

/* Callback when the position is updated. */
void (*bctrl_report_pos)(int16_t pos);

/* Magic byte that the factory-reset path checks for.
 * Set to 0x55 by the button state machine when both buttons
 * are held for 10 seconds. */
uint8_t bctrl_factory_magic;

/* 34-entry BCMD dispatch table, lifted from 0x1000-0x1043. */
typedef void (*bcmd_dispatch_fn)(void);
static const bcmd_dispatch_fn oem_bcmd_dispatch[34] = {
    [0]  = (bcmd_dispatch_fn)0x00cc,  /* BCMD state 0: do nothing */
    [1]  = (bcmd_dispatch_fn)0x00d5,  /* BCMD state 1: write 0x50 (default) */
    [2]  = (bcmd_dispatch_fn)0x00d5,  /* BCMD state 2: write 0x49 (init) */
    [3]  = (bcmd_dispatch_fn)0x00db,  /* BCMD state 3: write 0xca (UP fast) */
    [4]  = (bcmd_dispatch_fn)0x00e1,  /* BCMD state 4: write 0x8b (UP slow) */
    [5]  = (bcmd_dispatch_fn)0x00e7,  /* BCMD state 5: write 0x4c (DOWN slow) */
    [6]  = (bcmd_dispatch_fn)0x00ed,  /* BCMD state 6: write 0x0d (DOWN fast) */
    [7]  = (bcmd_dispatch_fn)0x00f3,  /* BCMD state 7: write 0x8e (DECEL) */
    /* States 8-12: no-op (return) */
    [8]  = (bcmd_dispatch_fn)0x00d4,
    [9]  = (bcmd_dispatch_fn)0x00d4,
    [10] = (bcmd_dispatch_fn)0x00d4,
    [11] = (bcmd_dispatch_fn)0x00d4,
    [12] = (bcmd_dispatch_fn)0x00d4,
    /* States 13-17: counter check (0x010e handler) */
    [13] = (bcmd_dispatch_fn)0x010e,
    [14] = (bcmd_dispatch_fn)0x010e,
    [15] = (bcmd_dispatch_fn)0x010e,
    [16] = (bcmd_dispatch_fn)0x010e,
    [17] = (bcmd_dispatch_fn)0x010e,
    /* State 18: no-op */
    [18] = (bcmd_dispatch_fn)0x00d4,
    /* States 19-34: post-move / endstop handlers at 0x0298-0x02b7 */
    [19] = (bcmd_dispatch_fn)0x0298,
    [20] = (bcmd_dispatch_fn)0x029b,
    [21] = (bcmd_dispatch_fn)0x029d,
    [22] = (bcmd_dispatch_fn)0x029f,
    [23] = (bcmd_dispatch_fn)0x02a1,
    [24] = (bcmd_dispatch_fn)0x02a3,
    [25] = (bcmd_dispatch_fn)0x02a5,
    [26] = (bcmd_dispatch_fn)0x02a7,
    [27] = (bcmd_dispatch_fn)0x02a9,
    [28] = (bcmd_dispatch_fn)0x02ab,
    [29] = (bcmd_dispatch_fn)0x02ad,
    [30] = (bcmd_dispatch_fn)0x02af,
    [31] = (bcmd_dispatch_fn)0x02b1,
    [32] = (bcmd_dispatch_fn)0x02b3,
    [33] = (bcmd_dispatch_fn)0x02b5,
};

/**
 * BCTRL state machine entry point.
 *
 * Lifted from the dispatch at 0x05C0 in the disassembly. The
 * state register (bctrl_state, RAM 0x4d) is consulted, and the
 * appropriate handler is called.
 *
 * The OEM uses a chain of `xorlw` + branch instructions rather
 * than a switch statement, but the C equivalent is clearer.
 */
void bctrl_state_machine(void) {
    switch (bctrl_state) {
        case BCTRL_STATE_IDLE:
            /* Default: do nothing. */
            break;

        case BCTRL_STATE_FACTORY_RESET_TRIGGER:
            /* Check for the 0x55 magic byte in RAM byte 0x4c. */
            if (bctrl_factory_magic == 0x55) {
                /* Magic byte seen — arm factory-reset. */
                bctrl_state = BCTRL_STATE_FACTORY_RESET_ARMED;
                /* Wait one tick for the next call. */
            } else {
                /* No magic — go back to idle and clear counters. */
                bctrl_state = BCTRL_STATE_IDLE;
                /* In OEM: clrf 0x53; incf 0x53 → 0x53 = 1 */
            }
            break;

        case BCTRL_STATE_FACTORY_RESET_ARMED:
            /* Clear the moving flag and call the BCMD dispatcher. */
            /* In OEM: clrf 0x4d; bsf 0x2b, 0x4; goto 0x00d3 */
            bctrl_state = BCTRL_STATE_IDLE;
            /* Trigger a STOP. */
            bcmd_state = BCMD_STATE_DEFAULT;
            break;

        case BCTRL_STATE_ENCODER_PRE_CHECK:
            /* Start sampling the encoder. */
            /* In OEM at 0x05F3: store 0xe3 in 0x6d, subwf 0x4e */
            bctrl_encoder_prev = bctrl_encoder;
            bctrl_state = BCTRL_STATE_ENCODER_ENDSTOP_CHECK;
            break;

        case BCTRL_STATE_ENCODER_ENDSTOP_CHECK:
            /* Compare current encoder to previous. If they are
             * equal, the encoder has stopped advancing — the legs
             * are stuck. Increment the stall counter; if it
             * exceeds the threshold, transition to ENDSTOP. */
            if (bctrl_encoder == bctrl_encoder_prev) {
                bctrl_stall_counter++;
                if (bctrl_stall_counter > ENDSTOP_STALL_TICKS) {
                    bctrl_state = BCTRL_STATE_ENDSTOP;
                }
            } else {
                bctrl_stall_counter = 0;
                bctrl_encoder_prev = bctrl_encoder;
            }
            break;

        case BCTRL_STATE_ENDSTOP:
            /* Send BCMD_STOP (0xfc) to release the relay. */
            /* In OEM at 0x0672: movlw 0xfc; clrf 0x4d */
            bcmd_state = BCMD_STATE_DEFAULT;  /* BCMD 0x50 (idle) */
            bctrl_stop_if_at_endstop();
            bctrl_state = BCTRL_STATE_IDLE;
            break;

        default:
            /* Default: do nothing (return). */
            break;
    }
}

/**
 * BCMD-byte dispatch entry point.
 *
 * Lifted from 0x1000-0x1043 in the disassembly. The state
 * register (bcmd_state, RAM 0x52) is consulted, and the
 * appropriate BCMD byte is written to register 0x61.
 */
void bcmd_dispatch(void) {
    if (bcmd_state < 34) {
        /* In the OEM, this is a computed goto via PCLATH. In C,
         * we just call the function pointer. */
        oem_bcmd_dispatch[bcmd_state]();
    }
}

/**
 * The endstop handler — called from state 6 of the BCTRL state
 * machine.
 *
 * Lifted from 0x0672 in the disassembly. The handler writes
 * BCMD_STOP (0xfc) to register 0x61, which causes the leg
 * controller to drop the relay.
 */
void bctrl_stop_if_at_endstop(void) {
    /* The OEM writes 0xfc to register 0x61 directly. */
    bcmd_state = BCMD_STATE_DEFAULT;
    /* In OEM: 0xfc is the "universal STOP" byte, written to
     * register 0x61. The LIN driver sends this byte to the
     * leg controller on the next bus frame. */
    /* (The actual write to the LIN buffer happens in the LIN
     * driver when the next frame is constructed.) */
}

/**
 * Callback used by endstop.c — registered as `orig_endstop_report`
 * by the user.c / InitApp code. This is the C bridge between the
 * endstop detector and the BCTRL state machine.
 */
void bctrl_stop_if_at_endstop_cb(int16_t pos) {
    (void)pos;
    bctrl_stop_if_at_endstop();
}

/**
 * Set the BCTRL target state. This is called by the BUI layer
 * when the user presses a button or the saved position matches
 * the current encoder.
 */
void bctrl_set_target(BCTRL_state_t s) {
    /* In the OEM, this updates a separate target state register. */
    (void)s;
}

/**
 * 50 ms scheduler tick — drives the BCTRL state machine.
 *
 * Lifted from the function at 0x3800-0x3be2 in the disassembly.
 * In the OEM, this is the Timer2 ISR + main loop's polling
 * point. The function calls the BCTRL state machine, then
 * the BCMD dispatch, then the LIN frame builder.
 */
void bctrl_timer(void) {
    bctrl_state_machine();
    bcmd_dispatch();
    /* The LIN frame builder runs in the LIN driver. */
    if (bctrl_report_pos) {
        bctrl_report_pos(bctrl_encoder);
    }
}

/**
 * Initialise the BCTRL state machine.
 */
void bctrl_init(void) {
    bctrl_state = BCTRL_STATE_IDLE;
    bcmd_state  = BCMD_STATE_DEFAULT;
    bctrl_encoder = 0;
    bctrl_encoder_prev = 0;
    bctrl_stall_counter = 0;
    bctrl_factory_magic = 0;

    /* Timer4: 4 MHz / 16 / 125 / 10 = 200 Hz = 5 ms.
     * Each bus frame slot is 5 ms; 20 slots = 100 ms cycle. */
    T4CONbits.T4CKPS  = 0b10;
    PR4 = 0x7d;
    T4CONbits.T4OUTPS = 0b1001;
    T4CONbits.TMR4ON  = 1;
    PIE3bits.TMR4IE   = 1;
}

/**
 * RX callback from the LIN driver.
 */
void bctrl_rx_lin(void) {
    /* No-op: the LIN schedule is fully timer-driven. */
}
