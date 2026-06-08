/**
 * @file user.c
 * @brief BEKANT controller — USART / LIN init recovered from the disassembly.
 *
 * The ivanwick project's USART setup matches the OEM firmware closely
 * (same baud rate generator, same TX/RX pin assignments). This file is
 * essentially a port of ivanwick's user.c with minor naming.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#if defined(__XC)
    #include <xc.h>
#elif defined(HI_TECH_C)
    #include <htc.h>
#endif

#include <stdint.h>
#include <stdbool.h>

#include "user.h"
#include "lin/lin_d.h"
#include "bekant/bctrl.h"
#include "bekant/bui.h"
#include "bekant/endstop.h"
#include "btn/btn.h"

static void InitUsart(void);

void InitApp(void) {
    /* OEM firmware order, recovered from the call chain at 0x0021:
     *   bctrl_init()      — set up Timer4 (LIN scheduler tick)
     *   bui_init()        — load memory positions from EEPROM
     *   orig_endstop_init — install the endstop detector
     *   btn_init()        — set up Timer2 + GPIO for buttons
     *   InitUsart()       — EUSART for LIN
     */
    bctrl_init();
    bui_init();
    orig_endstop_init();
    btn_init();

    /* Hook the LIN driver to bctrl, the position reports to bui, the
     * endstop detector to the LIN tick, and the button gestures to bui. */
    lin_frame_finish     = bctrl_rx_lin;
    bctrl_report_pos     = bui_set_pos;
    orig_endstop_report  = bctrl_stop_if_at_endstop_cb;
    btn_report_gesture   = bui_input;

    InitUsart();
}

static void InitUsart(void) {
    /* Identical to ivanwick-original: 16-bit baud generator,
     * inverted idle, async, 0x00cf baud register → 19200 @ 16 MHz. */
    RCSTAbits.SPEN = 0;

    TXSTAbits.SYNC = 0;   // async
    TXSTAbits.BRGH = 1;   // high speed
    BAUDCONbits.BRG16 = 1;

    SPBRGL = 0xcf;
    SPBRGH = 0x00;

    BAUDCONbits.WUE   = 0;
    BAUDCONbits.ABDEN = 0;
    TXSTAbits.SENDB   = 0;
    RCSTAbits.ADDEN   = 0;
    RCSTAbits.CREN    = 1;
    TXSTAbits.TXEN    = 0;  // enabled later, after main() finishes init

    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;

    INTCONbits.PEIE = 1;
    INTCONbits.GIE  = 1;

    BAUDCONbits.SCKP = 1;  // inverted idle for LIN
    RCSTAbits.SPEN   = 1;
}
