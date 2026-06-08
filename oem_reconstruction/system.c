/**
 * @file system.c
 * @brief BEKANT controller — oscillator and basic system setup.
 *
 * Reconstructed from orginafirm.hex (PIC16LF1938). The startup code at
 * 0x0221 in the disassembly sets:
 *   - 0x70/0x71 = 0x00 (state-machine scratch)
 *   - 0x04 = 0x21, 0x05 = 0x00 (16-bit constant 0x0021)
 *   - calls 0x0046 with those arguments (a 4-byte-store helper)
 *   - 0x04 = 0xa0, 0x05 = 0x00, W = 0x4c, call 0x0046 again
 *   - 0x04 = 0x20, 0x05 = 0x01, W = 0x39, call 0x0046
 *   - 0x04 = 0xd2, 0x05 = 0x01, W = 0x1a, call 0x0046
 *
 * These are EEPROM writes — the same four 16-bit constants are stored
 * at EEPROM addresses 0x0021, 0x00a0, 0x0120, 0x01d2. They are the
 * default memory positions for the two legs (high + low, two per leg).
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "system.h"

void ConfigureOscillator(void) {
    // IRCF<6:0> = 1111 (16 MHz HFINTOSC)
    // SCS<1:0> = 00 (use IRCF rather than external)
    OSCCONbits.IRCF = 0b1111;
    OSCCONbits.SCS  = 0b00;
}
