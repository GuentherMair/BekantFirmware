/**
 * @file bctrl.h
 * @brief BEKANT controller — recovered 8-state BCTRL plane.
 */
#ifndef BCTRL_H
#define BCTRL_H

#include <stdint.h>

/**
 * The 6 OEM BCTRL states (plus default).
 *
 * Note: the original `docs/ENDSTOP_ANALYSIS.md` describes the
 * state machine as having "8 states" — this is a count of
 * state values 0-7, but states 5 and 7+ are no-ops (the
 * default handler at 0x068e just returns). The 6 effective
 * states are:
 */
typedef enum {
    BCTRL_STATE_IDLE = 0,                    /* Default, no-op */
    BCTRL_STATE_FACTORY_RESET_TRIGGER = 1,   /* Check for 0x55 magic byte */
    BCTRL_STATE_FACTORY_RESET_ARMED  = 2,    /* Factory-reset about to fire */
    BCTRL_STATE_ENCODER_PRE_CHECK   = 3,     /* Start sampling encoder */
    BCTRL_STATE_ENCODER_ENDSTOP_CHECK = 4,   /* Sample-and-compare */
    BCTRL_STATE_ENDSTOP             = 6,     /* Endstop detected → STOP */
} BCTRL_state_t;

/* ivanwick-compatible aliases used by bui.c and other downstream
 * code. The ivanwick enum was: BCTRL_STOP=0, BCTRL_UP, BCTRL_DOWN,
 * BCTRL_CLICK, etc. — we re-export those names so the existing
 * bui.c still compiles. */
#define BCTRL_STOP   BCTRL_STATE_IDLE
#define BCTRL_UP     BCTRL_STATE_ENCODER_PRE_CHECK    /* pre-check = start UP */
#define BCTRL_DOWN   BCTRL_STATE_ENCODER_ENDSTOP_CHECK /* endstop = DOWN ? */
#define BCTRL_CLICK  BCTRL_STATE_FACTORY_RESET_TRIGGER /* placeholder */

/**
 * BCMD states (for the BCMD byte lookup).
 */
typedef enum {
    BCMD_STATE_DEFAULT = 0,   /* 0x50 (idle) */
    BCMD_STATE_1,             /* 0x50 (default) */
    BCMD_STATE_INIT,          /* 0x49 (init) */
    BCMD_STATE_UP_FAST,       /* 0xca (UP fast) */
    BCMD_STATE_UP_SLOW,       /* 0x8b (UP slow) */
    BCMD_STATE_DOWN_SLOW,     /* 0x4c (DOWN slow) */
    BCMD_STATE_DOWN_FAST,     /* 0x0d (DOWN fast) */
    BCMD_STATE_DECEL,         /* 0x8e (DECEL) */
    BCMD_STATE_PRE_STOP,      /* 0xcf (PRE_STOP) */
} BCMD_state_t;

/** Endstop stall threshold: how many 5 ms ticks the encoder
 *  must be stuck before triggering a STOP. */
#define ENDSTOP_STALL_TICKS  70

/** The factory-reset magic byte that the BCTRL state checks
 *  for in the button-driven code. */
#define FACTORY_RESET_MAGIC  0x55

/** State register, lifted from RAM 0x4d. */
extern BCTRL_state_t bctrl_state;

/** BCMD state register, lifted from RAM 0x52. */
extern BCMD_state_t bcmd_state;

/** Current encoder reading, lifted from RAM. */
extern int16_t bctrl_encoder;

/** Previous encoder reading (for endstop detection). */
extern int16_t bctrl_encoder_prev;

/** Stall counter. */
extern uint8_t bctrl_stall_counter;

/** Factory-reset magic byte (set by the button code). */
extern uint8_t bctrl_factory_magic;

/** Callback when the position is updated. */
extern void (*bctrl_report_pos)(int16_t pos);

/** Run the BCTRL state machine. */
void bctrl_state_machine(void);

/** Run the BCMD byte dispatch. */
void bcmd_dispatch(void);

/** Send BCMD_STOP (0xfc) to release the relay. */
void bctrl_stop_if_at_endstop(void);

/** Set the BCTRL target state (called by BUI). */
void bctrl_set_target(BCTRL_state_t s);

/** 50 ms scheduler tick. */
void bctrl_timer(void);

/** LIN RX callback. */
void bctrl_rx_lin(void);

/** Initialise the BCTRL state machine. */
void bctrl_init(void);

#endif /* BCTRL_H */
