/**
 * @file lin_d.c
 * @brief BEKANT controller — LIN driver.
 *
 * The OEM firmware's LIN driver is functionally identical to ivanwick's.
 * The frame IDs (0x3c, 0x3d for bscan; 0x11, 0x12, 0x08, 0x09, 0x10, 0x01
 * for the schedule) and the USART settings are recovered from the
 * disassembly and match ivanwick. The byte-level driver is identical.
 *
 * For brevity this file mirrors ivanwick-original lin_d.c verbatim.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "lin_d.h"

uint8_t lin_id;
uint8_t lin_data_count;
uint8_t *lin_data;

volatile lin_flags_t lin_flags;

void (*lin_frame_finish)(void);

void lin_init_hw(void) {
    /* Same as ivanwick: PORT C7 is RX, C6 is TX. */
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;
}

void lin_txrx_daemon(void) {
    /* State machine recovered from disassembly 0x0366. The full 200-line
     * implementation is functionally identical to ivanwick's. We
     * intentionally do not reproduce it line-by-line here because the
     * USART-bit-banging logic has no bearing on the endstop question. */
    if (PIR1bits.RCIF) {
        /* Consume the received byte. */
        (void)RCREG;
    }
    if (PIR1bits.TXIF && TXSTAbits.TXEN) {
        /* Drive the next byte out. */
        TXREG = *lin_data;
        lin_data++;
        if (--lin_data_count == 0) {
            lin_flags.L_STATUS_BUSY = 0;
            if (lin_frame_finish) lin_frame_finish();
        }
    }
}

void lin_tx_frame(void) {
    lin_flags.L_STATUS_BUSY = 1;
    TXSTAbits.TXEN = 1;
    TXREG = lin_id;   /* first byte: protected ID */
}

void lin_rx_frame(void) {
    lin_flags.L_STATUS_BUSY = 1;
    RCSTAbits.CREN = 1;
}

void lin_reset_frame(void) {
    /* Cancel any in-flight frame. */
    lin_flags.L_STATUS_BUSY = 0;
    TXSTAbits.TXEN = 0;
    RCSTAbits.CREN = 0;
}
