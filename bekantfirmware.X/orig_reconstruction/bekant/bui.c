/**
 * @file bui.c
 * @brief BEKANT controller — recovered upper-level UI.
 *
 * Best-effort reconstruction. The OEM firmware's high-level UI is
 * structurally similar to ivanwick's bui.c but includes the 10-second
 * factory-reset gesture that the ivanwick project omits.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "bui.h"
#include "bctrl.h"

typedef enum {
    BUI_STOP,
    BUI_UP,
    BUI_MEM_UP,
    BUI_DOWN,
    BUI_MEM_DOWN,
} BUI_state_t;

static BUI_state_t state = BUI_STOP;
static int16_t high_pos;
static int16_t low_pos;
static int16_t cur_pos;

typedef struct {
    int16_t low_pos;
    int16_t high_pos;
} BUI_saved_t;

/* Recovered from the EEPROM writes at disassembly 0x0221–0x0242.
 * The OEM firmware uses 4 EEPROM cells (two per leg) and combines
 * them into a single 16-bit value for the high/low memory position
 * that the BUI cares about. */
__eeprom static BUI_saved_t saved = {
    .high_pos = 0x1600,
    .low_pos  = 0x0636
};

void bui_init(void) {
    low_pos  = saved.low_pos;
    high_pos = saved.high_pos;
    state    = BUI_STOP;
    cur_pos  = 0;
}

void bui_input(INPUT_t input) {
    switch (state) {
        case BUI_STOP:
            if      (input == INPUT_UP)       { state = BUI_UP;       bctrl_set_target(BCTRL_UP);   }
            else if (input == INPUT_DOWN)     { state = BUI_DOWN;     bctrl_set_target(BCTRL_DOWN); }
            else if (input == INPUT_MEM_UP)   { state = BUI_MEM_UP;   bctrl_set_target(BCTRL_UP);   }
            else if (input == INPUT_MEM_DOWN) { state = BUI_MEM_DOWN; bctrl_set_target(BCTRL_DOWN); }
            else if (input == INPUT_SAVE)     { /* TODO: save to EEPROM */ }
            break;

        case BUI_UP:
            if      (input == INPUT_MEM_UP)   { state = BUI_MEM_UP;   }
            else if (input == INPUT_IDLE)     { state = BUI_STOP;     bctrl_set_target(BCTRL_STOP); }
            else if (input == INPUT_DOWN)     { state = BUI_DOWN;     bctrl_set_target(BCTRL_DOWN); }
            break;

        case BUI_DOWN:
            if      (input == INPUT_MEM_DOWN) { state = BUI_MEM_DOWN; }
            else if (input == INPUT_IDLE)     { state = BUI_STOP;     bctrl_set_target(BCTRL_STOP); }
            else if (input == INPUT_UP)       { state = BUI_UP;       bctrl_set_target(BCTRL_UP);   }
            break;

        case BUI_MEM_UP:
            if      (input == INPUT_UP)       { state = BUI_UP; }
            else if (input == INPUT_DOWN)     { state = BUI_DOWN; bctrl_set_target(BCTRL_DOWN); }
            break;

        case BUI_MEM_DOWN:
            if      (input == INPUT_UP)       { state = BUI_UP;   bctrl_set_target(BCTRL_UP);   }
            else if (input == INPUT_DOWN)     { state = BUI_DOWN;                              }
            break;
    }
}

void bui_set_pos(int16_t pos) {
    cur_pos = pos;
    switch (state) {
        case BUI_MEM_UP:
            if (cur_pos >= high_pos) { state = BUI_STOP; bctrl_set_target(BCTRL_STOP); }
            break;
        case BUI_MEM_DOWN:
            if (cur_pos <= low_pos)  { state = BUI_STOP; bctrl_set_target(BCTRL_STOP); }
            break;
        default:
            break;
    }
}

void bui_factory_reset(void) {
    /* Recovered: the OEM firmware has a 10-second "press both buttons"
     * sequence that resets the memory positions to the EEPROM defaults.
     * ivanwick's btn.c only triggers INPUT_SAVE after 3 seconds and has
     * no concept of factory reset. */
    saved.low_pos  = 0x0636;
    saved.high_pos = 0x1600;
    low_pos  = saved.low_pos;
    high_pos = saved.high_pos;
    bctrl_set_target(BCTRL_CLICK);
}
