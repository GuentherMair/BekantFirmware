
#include <stdint.h>

typedef enum {
    BCTRL_AFTER_SCAN,
    BCTRL_STOP,
    BCTRL_PRE_MOVE,
    BCTRL_UP,
    BCTRL_DOWN,
    BCTRL_UP_DECEL,
    BCTRL_DOWN_DECEL,
    BCTRL_PRE_STOP,
    BCTRL_CLICK,
} BCTRL_state_t;

/* OEM BCMD_* values recovered from disassembly 0x0d5–0x0ff.
 * Only BCMD_STOP_IDLE (0xfc) matches ivanwick — the rest are
 * the OEM values, not the ivanwick guesses. */
enum {
    BCMD_AFTER_SCAN_INIT = 0x50,  /* OEM default: state 0/1 → 0x50 */
    BCMD_STOP_IDLE       = 0xfc,  /* OEM: universal STOP (drops relay) */
    BCMD_PRE_MOVE        = 0x49,  /* OEM: state 2 → 0x49 */
    BCMD_UP_FAST         = 0xca,  /* OEM: state 3 → 0xca */
    BCMD_UP_SLOW         = 0x8b,  /* OEM: state 4 → 0x8b */
    BCMD_DOWN_SLOW       = 0x4c,  /* OEM: state 5 → 0x4c */
    BCMD_DOWN_FAST       = 0x0d,  /* OEM: state 6 → 0x0d */
    BCMD_DECEL           = 0x8e,  /* OEM: state 7 → 0x8e */
    BCMD_PRE_STOP        = 0xcf,  /* OEM: state 8 → 0xcf */
};

void bctrl_timer(void);
void bctrl_set_target(BCTRL_state_t state);
void bctrl_rx_lin(void);
void bctrl_init(void);

/* Current position (lifted from RAM, populated by bctrl_timer). */
extern int16_t bctrl_pos;
/* Current and target state (used by endstop detector). */
extern BCTRL_state_t current_state;
extern BCTRL_state_t target_state;

extern void (*bctrl_report_pos)(int16_t pos);

// BUI also needs this value so it can anticipate stops
#define BCTRL_DECEL_MARGIN 0x6c
