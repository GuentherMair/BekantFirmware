#ifndef BCTRL_H
#define BCTRL_H

#include <stdint.h>

/* BCTRL state machine — recovered from the disassembly. The OEM
 * firmware uses a different state space from ivanwick; see the
 * reconstructed bctrl.c for the mapping. */
typedef enum {
    BCTRL_AFTER_SCAN,
    BCTRL_STOP,         /* also used as "idle" / endstop-detected */
    BCTRL_PRE_MOVE,
    BCTRL_UP,           /* moving up */
    BCTRL_DOWN,         /* moving down */
    BCTRL_UP_DECEL,     /* overshoot target while moving up */
    BCTRL_DOWN_DECEL,   /* overshoot target while moving down */
    BCTRL_PRE_STOP,     /* about to stop */
    BCTRL_CLICK,        /* one-tick relay click for audible feedback */
} BCTRL_state_t;

void bctrl_init(void);
void bctrl_timer(void);
void bctrl_set_target(BCTRL_state_t state);
void bctrl_rx_lin(void);

/* OEM BCMD_* values recovered from disassembly 0x0d5–0x0ff.
 * 0xfc is the universal STOP — the leg controller drops the relay. */
enum {
    BCMD_AFTER_SCAN_INIT  = 0x50,  /* OEM default: state 0/1 → 0x50 */
    BCMD_STOP_IDLE        = 0xfc,  /* OEM: same as ivanwick — STOP */
    BCMD_PRE_MOVE         = 0x49,  /* OEM: state 2 → 0x49 */
    BCMD_UP_FAST          = 0xca,  /* OEM: state 3 → 0xca */
    BCMD_UP_SLOW          = 0x8b,  /* OEM: state 4 → 0x8b */
    BCMD_DOWN_SLOW        = 0x4c,  /* OEM: state 5 → 0x4c */
    BCMD_DOWN_FAST        = 0x0d,  /* OEM: state 6 → 0x0d */
    BCMD_DECEL            = 0x8e,  /* OEM: state 7 → 0x8e */
    BCMD_PRE_STOP         = 0xcf,  /* OEM: state 8 → 0xcf */
    /* The OEM firmware does NOT use ivanwick-style ERR_UP / ERR_DOWN
     * commands. Endstop detection goes through the separate module
     * orig_endstop.c. */
};

/* BCTRL reports the new position to the registered callback. The
 * bctrl.c registers this to orig_endstop_report (in user.c) so the
 * endstop detector sees every position update. */
extern void (*bctrl_report_pos)(int16_t pos);

#endif /* BCTRL_H */
