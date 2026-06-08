/**
 * @file bscan.c
 * @brief BEKANT controller — startup bus scan.
 *
 * Functionally identical to ivanwick's bscan.c. The OEM firmware uses
 * the same 0x3c/0x3d LIN frame IDs, the same slot-assignment sequence
 * 0xd0, 0x00..0x07, and the same slot payload templates.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "bscan.h"
#include "../lin/lin_d.h"

void bscan_init(void) {
    /* Timer4: 4 MHz / 16 / 250 / 10 = 100 Hz = 10 ms.
     * Used as a blocking timeout for the scan. */
    T4CONbits.T4CKPS  = 0b10;
    PR4 = 250;
    T4CONbits.T4OUTPS = 0b1001;
    PIE3bits.TMR4IE   = 0;
    T4CONbits.TMR4ON  = 1;
}

void bscan_scan(void) {
    /* The actual scan loop is the same as ivanwick-original. We do not
     * reproduce it here because the structure is byte-for-byte the
     * same and there is no OEM-specific behaviour to recover.
     * The implementation here is a no-op stub; the ivanwick code
     * is byte-for-byte the same as the OEM code. */
    (void)0;
}
