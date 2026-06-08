/**
 * @file bcmd_table.c
 * @brief BEKANT controller — BCMD state-indexed lookup.
 *
 * The OEM firmware has 8 distinct BCMD (LIN command) bytes, one
 * per state. They are emitted by the BCMD state machine (0x00d5-0x00ff
 * in the disassembly) which uses a chain of `subwf` + branch
 * instructions to map the current state (RAM byte 0x52) to a
 * specific BCMD byte, then writes the result to RAM byte 0x61.
 *
 * The mapping recovered from the disassembly is:
 *
 *   state 0, 1 (default)  → 0x50  (idle / not moving)
 *   state 2              → 0x49  (init / "wake" the bus)
 *   state 3              → 0xca  (move up — fast)
 *   state 4              → 0x8b  (move up — slow)
 *   state 5              → 0x4c  (move down — slow)
 *   state 6              → 0x0d  (move down — fast)
 *   state 7              → 0x8e  (decelerate / overshoot)
 *   state 8              → 0xcf  (pre-stop / "almost there")
 *   (universal STOP)     → 0xfc  (sent by endstop handler at 0x0678)
 *
 * Compare with ivanwick's BCMD_* constants in src/bekant/bctrl.c:
 *   ivanwick BCMD_UP     = 0x86  (OEM = 0xca / 0x8b, two-speed)
 *   ivanwick BCMD_DOWN   = 0x85  (OEM = 0x4c / 0x0d, two-speed)
 *   ivanwick BCMD_DECEL  = 0x87  (OEM = 0x8e)
 *   ivanwick BCMD_PRE_STOP = 0x84  (OEM = 0xcf)
 *   ivanwick BCMD_PRE_MOVE = 0xc4  (OEM = 0x49)
 *   ivanwick BCMD_STOP_IDLE = 0xfc  (OEM = 0xfc — MATCH)
 *
 * Only 0xfc matches. The other ivanwick values are guesses that
 * work in practice because the LIN leg controller is forgiving.
 *
 * This file recovers the OEM mapping in two equivalent forms:
 *   1. A const lookup table (data-oriented — easier to read)
 *   2. A switch statement (control-flow oriented — closer to the
 *      OEM assembly shape)
 */

#include "bcmd_table.h"

/* ivanwick's guesses, kept here for comparison. */
const uint8_t ivanwick_bcmd_table[9] = {
    [BCMD_AFTER_SCAN_INIT] = 0xbf,
    [BCMD_STOP_IDLE]       = 0xfc,
    [BCMD_PRE_MOVE]        = 0xc4,
    [BCMD_UP]              = 0x86,
    [BCMD_DOWN]            = 0x85,
    [BCMD_DECEL]           = 0x87,
    [BCMD_PRE_STOP]        = 0x84,
    [BCMD_ERR_UP]          = 0xbc,
    [BCMD_ERR_DOWN]        = 0xbd,
};

/* Recovered OEM mapping. */
const uint8_t oem_bcmd_table[9] = {
    [0] = 0x50,  /* default (idle / not moving) */
    [1] = 0x50,  /* also default */
    [2] = 0x49,  /* init / wake bus */
    [3] = 0xca,  /* move up — fast */
    [4] = 0x8b,  /* move up — slow */
    [5] = 0x4c,  /* move down — slow */
    [6] = 0x0d,  /* move down — fast */
    [7] = 0x8e,  /* decelerate / overshoot */
    [8] = 0xcf,  /* pre-stop / "almost there" */
};

/* Universal STOP byte, used by the endstop handler. */
const uint8_t BCMD_STOP_UNIVERSAL = 0xfc;

/**
 * Switch-based form of the OEM BCMD lookup, faithful to the
 * disassembly shape at 0x00d5-0x00ff.
 */
uint8_t bcmd_lookup_switch(uint8_t state) {
    uint8_t bcmd;
    if      (state == 2) bcmd = 0x49;
    else if (state == 3) bcmd = 0xca;
    else if (state == 4) bcmd = 0x8b;
    else if (state == 5) bcmd = 0x4c;
    else if (state == 6) bcmd = 0x0d;
    else if (state == 7) bcmd = 0x8e;
    else if (state == 8) bcmd = 0xcf;
    else                 bcmd = 0x50;
    return bcmd;
}
