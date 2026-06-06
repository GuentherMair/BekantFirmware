/**
 * @file interrupts.c
 * @brief BEKANT controller — recovered ISR dispatch.
 *
 * From the disassembly, three IRQ sources are visible:
 *   - PIR1.RCIF    → USART RX byte → calls 0x0037 (the LIN tx/rx daemon)
 *   - PIR3.TMR4IF  → Timer4 overflow → calls 0x0021 (the bctrl timer)
 *   - PIR1.TMR2IF  → Timer2 overflow → calls 0x0051 (the btn timer)
 *
 * The order of the if/else chain matches the ivanwick project; the call
 * targets are the recovered OEM addresses.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "lin/lin_d.h"
#include "bekant/bctrl.h"
#include "btn/btn.h"

void isr(void) {
    if (PIR1bits.RCIF) {
        lin_txrx_daemon();
        PIR1bits.RCIF = 0;
    } else if (PIR3bits.TMR4IF) {
        bctrl_timer();
        PIR3bits.TMR4IF = 0;
    } else if (PIR1bits.TMR2IF) {
        btn_timer();
        PIR1bits.TMR2IF = 0;
    }
}
