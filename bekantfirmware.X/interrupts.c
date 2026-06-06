#include <xc.h> // XC8 General Include File

/* For syntax-only checks without the real XC8 PIC SFR headers,
 * BEKANT_TEST_SYNTAX pulls in a stub. */
#ifdef BEKANT_TEST_SYNTAX
#include "test_sfr_stubs.h"
#endif

#include <stdint.h>
#include <stdbool.h>

#include "lin/lin_d.h"
#include "bekant/bctrl.h"
#include "btn/btn.h"

void isr(void)
{
    if (PIR1bits.RCIF) {
        lin_txrx_daemon();
        PIR1bits.RCIF = 0;
    } else if (PIR3bits.TMR4IF) {
        bctrl_timer();
        PIR3bits.TMR4IF = false;
    } else if (PIR1bits.TMR2IF) {
        btn_timer();
        PIR1bits.TMR2IF = false;
    }
}
