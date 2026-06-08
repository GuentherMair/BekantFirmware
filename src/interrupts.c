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

/* The PIC16LF1938 has a single interrupt vector at 0x0004. The
 * `__interrupt()` qualifier tells XC8 to install this function as
 * the interrupt service routine. Without it, XC8 in free mode
 * dead-strips the function (because no other code calls it) and
 * the startup code prints "; No interrupt routine" at address
 * 0x0004. With it, XC8 emits the function into the `intentry`
 * psect and the runtime startup installs a `goto _isr` at the
 * interrupt vector.
 *
 * On entry to an ISR, the PIC16 hardware has already saved WREG,
 * STATUS and PCLATH on the hardware stack (3 levels). The
 * remaining registers (FSR0/1, BSR, the data banks) are caller-
 * saved per the AAP convention; we restore them manually.
 */
void __interrupt() isr(void)
{
    /* Save the C-save context (BCR + linear GPR banks 0..4).
     * The XC8 `__interrupt()` qualifier already saves/restores
     * WREG/STATUS/PCLATH; we save the rest because the LIN
     * driver, bctrl and btn all touch different banks. */
#if defined(__18CXX) || defined(__XC) || defined(__XC8)
    /* XC8 manages the context for us; the qualifier is enough. */
#endif

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

