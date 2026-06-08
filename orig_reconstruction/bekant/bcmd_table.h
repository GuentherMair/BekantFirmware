/**
 * @file bcmd_table.h
 * @brief Recovered BCMD lookup table from the OEM firmware.
 */
#ifndef BCMD_TABLE_H
#define BCMD_TABLE_H

#include <stdint.h>

/**
 * ivanwick's BCMD_* enum names, kept for direct comparison.
 * These do NOT match the OEM values; only BCMD_STOP_IDLE = 0xfc
 * is correct.
 */
typedef enum {
    BCMD_AFTER_SCAN_INIT = 0,
    BCMD_STOP_IDLE       = 1,
    BCMD_PRE_MOVE        = 2,
    BCMD_UP              = 3,
    BCMD_DOWN            = 4,
    BCMD_DECEL           = 5,
    BCMD_PRE_STOP        = 6,
    BCMD_ERR_UP          = 7,
    BCMD_ERR_DOWN        = 8,
} ivanwick_bcmd_t;

/** ivanwick's BCMD byte values. Only BCMD_STOP_IDLE matches the OEM. */
extern const uint8_t ivanwick_bcmd_table[9];

/** Recovered OEM BCMD byte values, indexed by state. */
extern const uint8_t oem_bcmd_table[9];

/** Universal STOP byte, used by the endstop handler. */
extern const uint8_t BCMD_STOP_UNIVERSAL;

/** Switch-based form of the OEM BCMD lookup, faithful to the disassembly. */
uint8_t bcmd_lookup_switch(uint8_t state);

#endif /* BCMD_TABLE_H */
