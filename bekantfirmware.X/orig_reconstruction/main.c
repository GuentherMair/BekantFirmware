/**
 * @file main.c
 * @brief BEKANT controller — recovered startup sequence.
 *
 * Reconstructed from orginafirm.hex (PIC16LF1938). The reset vector at
 * 0x0000 → goto 0x001f → goto 0x0221 → goto 0x0185 → goto 0x0190
 * yields the main loop entry at 0x0190. The function 0x0221 is the
 * init-and-start sequence: it stores four 16-bit constants into EEPROM
 * (the default memory positions) and then falls into the main loop.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "system.h"
#include "user.h"
#include "lin/lin_d.h"
#include "bekant/bscan.h"
#include "bekant/bctrl.h"
#include "bekant/bui.h"
#include "bekant/orig_endstop.h"
#include "btn/btn.h"

void main(void) {
    ConfigureOscillator();
    InitApp();

    /* Same as ivanwick-original main.c:
     *   ANSELB = 0; TRISB = 0b00000011;   (RB0/1 are the two buttons)
     *   LATC   = 1; TRISC = 0b10000000;   (C7 is LIN RX, rest are output) */
    ANSELB = 0x00;
    TRISB  = 0b00000011;
    LATC   = 1;
    TRISC  = 0b10000000;

    lin_init_hw();

    /* OEM firmware runs a startup bus scan (bscan_scan) before letting
     * the bctrl loop take over. The ivanwick project kept the same
     * structure but uses different frame IDs. */
    bscan_init();
    bscan_scan();

    bctrl_init();
    bui_init();
    orig_endstop_init();
    btn_init();

    while (1) {
        CLRWDT();
    }
}
