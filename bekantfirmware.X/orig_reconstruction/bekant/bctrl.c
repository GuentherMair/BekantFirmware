/**
 * @file bctrl.c
 * @brief BEKANT controller — recovered LIN control plane.
 *
 * This is a best-effort reconstruction. The OEM firmware's BCTRL
 * state machine is more elaborate than ivanwick's — it has 8
 * distinct BCMD outputs and a state-indexed lookup table (recovered
 * at disassembly 0x0d5–0x0ff) that maps the current state to the
 * BCMD byte written to register 0x61.
 *
 * For the rest (schedule, decoder, etc.) we reuse ivanwick's logic
 * because the LIN frame IDs and the byte-level LIN protocol are
 * identical.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bctrl.h"
#include "orig_endstop.h"
#include "../lin/lin_d.h"

void (*bctrl_report_pos)(int16_t pos);
static void bctrl_populate_cmd(void);
static void bctrl_next_state(void);

int16_t  bctrl_pos;
BCTRL_state_t current_state = BCTRL_AFTER_SCAN;
BCTRL_state_t target_state  = BCTRL_STOP;

static uint8_t  decel_counter = 0;
static int16_t  decel_target  = 0;

typedef union {
    uint8_t data[3];
    struct {
        int16_t encoder;
        uint8_t status;
    };
} BCTRL_bus_data_t;

static BCTRL_bus_data_t zeroes     = { 0, 0, 0 };
static BCTRL_bus_data_t data_08;
static BCTRL_bus_data_t data_09;
static BCTRL_bus_data_t cmd_data;

/* Recovered LIN bus schedule. Same shape as ivanwick; the bcmd byte
 * (cmd_data.status) is what we changed in the populate_cmd() function. */
static struct {
    uint8_t           id;
    uint8_t           data_count;
    bool              tx;
    BCTRL_bus_data_t *data;
} schedule[] = {
    { 0x11, 3, true,  &zeroes    },
    { 0x08, 3, false, &data_08   },
    { 0x09, 3, false, &data_09   },
    { 0x10, 0, false, NULL       },
    { 0x10, 0, false, NULL       },
    { 0x10, 0, false, NULL       },
    { 0x10, 0, false, NULL       },
    { 0x10, 0, false, NULL       },
    { 0x10, 0, false, NULL       },
    { 0x01, 0, false, NULL       },
    { 0x12, 3, true,  &cmd_data  },
};

#define SCHEDULE_OCCUPIED_COUNT  11
#define SCHEDULE_TOTAL_COUNT     20
#define SCHEDULE_COMMAND_SLOT    10
#define DECEL_COUNT_MAX          3

void bctrl_set_target(BCTRL_state_t s) {
    switch (s) {
        case BCTRL_STOP:
            target_state = s;
            break;
        case BCTRL_UP:
        case BCTRL_DOWN:
        case BCTRL_CLICK:
            target_state = s;
            break;
        default:
            /* Don't allow intermediate states as targets. */
            break;
    }
}

static void bctrl_next_state(void) {
    switch (current_state) {
        case BCTRL_AFTER_SCAN:
            current_state = BCTRL_STOP;
            break;

        case BCTRL_STOP:
            if (target_state == BCTRL_UP || target_state == BCTRL_DOWN
                    || target_state == BCTRL_CLICK) {
                current_state = BCTRL_PRE_MOVE;
            }
            break;

        case BCTRL_PRE_MOVE:
            if (target_state == BCTRL_UP || target_state == BCTRL_DOWN
                    || target_state == BCTRL_CLICK) {
                current_state = target_state;
            }
            break;

        case BCTRL_UP:
            if (target_state != current_state) {
                current_state = BCTRL_UP_DECEL;
                decel_counter = 0;
            }
            break;
        case BCTRL_DOWN:
            if (target_state != current_state) {
                current_state = BCTRL_DOWN_DECEL;
                decel_counter = 0;
            }
            break;

        case BCTRL_UP_DECEL:
        case BCTRL_DOWN_DECEL:
            if (++decel_counter >= DECEL_COUNT_MAX) {
                current_state = BCTRL_PRE_STOP;
            }
            break;

        case BCTRL_PRE_STOP:
            current_state = BCTRL_STOP;
            break;

        case BCTRL_CLICK:
            current_state = BCTRL_PRE_STOP;
            bctrl_set_target(BCTRL_STOP);
            break;
    }
}

/**
 * Populate the LIN command frame based on the current state.
 *
 * This function is the most-recovered piece of OEM behaviour: the
 * state-indexed table at disassembly 0x0d5–0x0ff writes a single
 * BCMD byte (0x50, 0x49, 0xca, 0x8b, 0x4c, 0x0d, 0x8e, or 0xcf) to
 * register 0x61. The encoder value sent along with the BCMD comes
 * from the same logic as ivanwick (min of the two leg encoders for
 * UP, max for DOWN).
 */
static void bctrl_populate_cmd(void) {
    int16_t enc_min = (data_08.encoder < data_09.encoder)
                        ? data_08.encoder : data_09.encoder;
    int16_t enc_max = (data_08.encoder > data_09.encoder)
                        ? data_08.encoder : data_09.encoder;
    bool error_cond = (data_08.encoder < 0 || data_09.encoder < 0);

    switch (current_state) {
        case BCTRL_AFTER_SCAN:
            cmd_data.encoder = (int16_t)0xfff6;
            cmd_data.status  = BCMD_AFTER_SCAN_INIT;  /* 0x50 in OEM */
            break;

        case BCTRL_UP_DECEL:
            if (decel_counter == 0) {
                decel_target = enc_max + 0x6c;     /* DECEL_MARGIN */
            }
            cmd_data.encoder = decel_target;
            cmd_data.status  = BCMD_DECEL;          /* 0x8e in OEM */
            break;

        case BCTRL_DOWN_DECEL:
            if (decel_counter == 0) {
                decel_target = enc_min - 0x6c;     /* DECEL_MARGIN */
            }
            cmd_data.encoder = decel_target;
            cmd_data.status  = BCMD_DECEL;          /* 0x8e in OEM */
            break;

        case BCTRL_UP:
            if (error_cond) {
                cmd_data.encoder = enc_max;
                cmd_data.status  = BCMD_UP_SLOW;    /* 0x8b in OEM */
            } else {
                cmd_data.encoder = enc_min;
                cmd_data.status  = BCMD_UP_FAST;    /* 0xca in OEM */
            }
            break;

        case BCTRL_DOWN:
            if (error_cond) {
                cmd_data.encoder = 0;
                cmd_data.status  = BCMD_DOWN_SLOW;  /* 0x4c in OEM */
            } else {
                cmd_data.encoder = enc_max;
                cmd_data.status  = BCMD_DOWN_FAST;  /* 0x0d in OEM */
            }
            break;

        case BCTRL_STOP:
            cmd_data.encoder = enc_max;
            cmd_data.status  = BCMD_STOP_IDLE;      /* 0xfc — universal STOP */
            break;

        case BCTRL_PRE_MOVE:
            cmd_data.encoder = enc_max;
            cmd_data.status  = BCMD_PRE_MOVE;       /* 0x49 in OEM */
            break;

        case BCTRL_PRE_STOP:
            cmd_data.encoder = enc_max;
            cmd_data.status  = BCMD_PRE_STOP;       /* 0xcf in OEM */
            break;

        case BCTRL_CLICK:
            /* One tick of DOWN_FAST to click the relay (brake). The
             * OEM firmware uses DOWN_FAST (0x0d) for this. */
            cmd_data.encoder = enc_max;
            cmd_data.status  = BCMD_DOWN_FAST;
            break;
    }
}

void bctrl_timer(void) {
    static uint8_t schedule_pos = 0;

    if (schedule_pos > 0) {
        uint8_t prior = schedule_pos - 1;
        if (prior < SCHEDULE_OCCUPIED_COUNT) {
            if (lin_flags.L_STATUS_BUSY) {
                lin_reset_frame();
            }
        }
    }

    if (schedule_pos < SCHEDULE_OCCUPIED_COUNT) {
        if (schedule_pos == SCHEDULE_COMMAND_SLOT) {
            bctrl_populate_cmd();
        }
        lin_id         = schedule[schedule_pos].id;
        lin_data_count = schedule[schedule_pos].data_count;
        lin_data       = (uint8_t *)schedule[schedule_pos].data;
        if (schedule[schedule_pos].tx) lin_tx_frame();
        else                            lin_rx_frame();
    }

    schedule_pos++;
    if (schedule_pos >= SCHEDULE_TOTAL_COUNT) {
        bctrl_pos = cmd_data.encoder;
        bctrl_report_pos(bctrl_pos);
        schedule_pos = 0;
        bctrl_next_state();
    }
}

void bctrl_rx_lin(void) {
    /* No-op: the LIN schedule is fully timer-driven. The endstop
     * detection lives in orig_endstop.c, not here. */
}

void bctrl_init(void) {
    /* Timer4: 4 MHz / 16 / 125 / 10 = 200 Hz = 5 ms.
     * Each bus frame slot is 5 ms; 20 slots = 100 ms cycle. */
    T4CONbits.T4CKPS  = 0b10;
    PR4bits.PR4       = 0x7d;
    T4CONbits.T4OUTPS = 0b1001;
    T4CONbits.TMR4ON  = 1;
    PIE3bits.TMR4IE   = 1;
}
