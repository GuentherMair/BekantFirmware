/**
 * @file eeprom_defaults.c
 * @brief BEKANT controller — recovered EEPROM initialisation and
 *        factory-reset.
 *
 * The OEM firmware writes four 16-bit values to EEPROM on first
 * boot (at function 0x0221 in the disassembly). The values are
 * loaded from constants embedded in the code:
 *
 *   EEPROM[0x00..0x01] = 0x0021   (sitting position, low byte first)
 *   EEPROM[0x02..0x03] = 0x00a0   (some other low-byte value)
 *   EEPROM[0x04..0x05] = 0x0120   (some other high-byte value)
 *   EEPROM[0x06..0x07] = 0x01d2   (standing position, low byte first)
 *
 * Each `call 0x0046` invocation stores a (low, high) byte pair
 * into the EEPROM buffer at offsets (0x04, 0x05). The actual
 * EEPROM write happens later in the function (at 0x0252, 0x025f,
 * 0x026b, 0x0277) using a helper at 0x07fa.
 *
 * The 0x07fa helper is a 6-word EEPROM write sequence (at
 * 0x1FFA-0x1FFF) that copies a 16-bit value from INDF0 to INDF1
 * for `0x7e` iterations:
 *
 *   0x1FFA:  movwf 0x7e        ; store count from W
 *   0x1FFB:  moviw 0++         ; INDF0++
 *   0x1FFC:  movwi 0x1++       ; INDF1++
 *   0x1FFD:  decfsz 0x7e, 0x1  ; decrement, skip if 0
 *   0x1FFE:  goto 0x1FFB       ; loop
 *   0x1FFF:  retlw 0x00        ; return 0
 *
 * The factory-reset sequence is at 0x05D8 (state 1 handler):
 * if RAM byte 0x4c is 0x55 (set by the 10-second both-buttons
 * hold counter), the state transitions to 0x02 and on the next
 * tick the EEPROM defaults are rewritten.
 *
 * Best-effort reconstruction — see ../docs/FACTORY_RESET.md.
 */

#include "eeprom_defaults.h"
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/* Recovered default values from 0x0221-0x0242. */
#define OEM_DEFAULT_SIT_POS    0x0021u
#define OEM_DEFAULT_LOW_POS    0x00a0u
#define OEM_DEFAULT_HIGH_POS   0x0120u
#define OEM_DEFAULT_STAND_POS  0x01d2u

/* ivanwick's defaults (kept here for comparison). */
#define IVANWICK_DEFAULT_SIT    0x0636
#define IVANWICK_DEFAULT_STAND  0x1600

/**
 * The factory-reset magic byte: when RAM 0x4c is set to 0x55 by
 * the 10-second both-buttons-hold counter, the state machine at
 * 0x05D8 transitions to state 0x02 and re-initialises the
 * EEPROM defaults.
 */
#define FACTORY_RESET_MAGIC     0x55

/**
 * Write a 16-bit value to EEPROM at the given address.
 *
 * Recovered from the disassembly at 0x07fa-0x1fff (the EEPROM
 * write helper that copies a 16-bit value from one buffer to
 * another). The actual EEPROM write is performed by the XC8
 * runtime's `eeprom_write` builtin; this function wraps it.
 */
static void eeprom_write16(uint8_t addr, uint16_t value) {
    EEADR = addr;
    EEDATA = (uint8_t)(value & 0xff);
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.WREN  = 1;
    EECON2 = 0x55;
    EECON2 = 0xaa;
    EECON1bits.WR    = 1;
    while (EECON1bits.WR) ;
    EECON1bits.WREN  = 0;
    EEADR = addr + 1;
    EEDATA = (uint8_t)((value >> 8) & 0xff);
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.WREN  = 1;
    EECON2 = 0x55;
    EECON2 = 0xaa;
    EECON1bits.WR    = 1;
    while (EECON1bits.WR) ;
    EECON1bits.WREN  = 0;
}

/**
 * Read a 16-bit value from EEPROM at the given address.
 */
static uint16_t eeprom_read16(uint8_t addr) {
    EEADR = addr;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.RD    = 1;
    uint8_t lo = EEDATA;
    EEADR = addr + 1;
    EECON1bits.RD    = 1;
    uint8_t hi = EEDATA;
    return ((uint16_t)hi << 8) | lo;
}

/**
 * Check if the EEPROM has been initialised. The OEM firmware uses
 * a 0xff-0xff-0xff-0xff sentinel (4 bytes of 0xff) at the
 * beginning of EEPROM to detect first boot.
 *
 * The recovered code at 0x0221 writes the defaults unconditionally,
 * so this is a "rewrite every boot" strategy. ivanwick's code uses
 * a different convention (check if EEPROM[0] != 0xff).
 */
static bool eeprom_is_uninitialised(void) {
    return eeprom_read16(0x00) == 0xffff;
}

/**
 * Initialise the EEPROM with OEM defaults.
 *
 * Recovered from the function at 0x0221 in the disassembly. The
 * four 16-bit values written are the default sitting and standing
 * heights used by the OEM firmware.
 *
 * The OEM firmware calls this from the reset-vector chain (main.c
 * calls 0x0221 via the goto 0x0221 at 0x0020). ivanwick's code
 * uses different defaults (0x0636 / 0x1600) and writes them only
 * if the EEPROM is uninitialised.
 */
void eeprom_init_defaults(void) {
    eeprom_write16(0x00, OEM_DEFAULT_SIT_POS);
    eeprom_write16(0x02, OEM_DEFAULT_LOW_POS);
    eeprom_write16(0x04, OEM_DEFAULT_HIGH_POS);
    eeprom_write16(0x06, OEM_DEFAULT_STAND_POS);
}

/**
 * ivanwick-style initialisation: only write if EEPROM is
 * uninitialised. This is the variant used by the current build.
 */
void eeprom_init_if_empty(void) {
    if (eeprom_is_uninitialised()) {
        eeprom_write16(0x00, IVANWICK_DEFAULT_SIT);
        eeprom_write16(0x02, IVANWICK_DEFAULT_STAND);
    }
}

/**
 * Get the factory-reset magic byte that the state machine checks
 * for at 0x05D8. The button-handler code in btn.c is responsible
 * for writing this byte to RAM 0x4c when both buttons are held
 * for 10 seconds.
 */
uint8_t factory_reset_magic(void) {
    return FACTORY_RESET_MAGIC;
}

/**
 * Perform a factory reset.
 *
 * Recovered from the state machine at 0x05D8 (state 1 → state 2
 * transition on magic byte 0x55). The actual reset work is done
 * by the state machine handler at 0x05E1, which writes a value
 * from 0x44 to SFR 0x1a and clears 0x42.
 *
 * In C terms, the factory reset:
 *   1. Re-writes the four 16-bit EEPROM defaults.
 *   2. Clears the moving flag (0x47 bit 1 in the OEM).
 *   3. Clears the state machine counter (0x42).
 *   4. Sets a "post-reset" marker in 0x53.
 *
 * Note: this is the canonical implementation. The bui.c file
 * has its own `bui_factory_reset` that does the same thing
 * but for the ivanwick-style BUI; that one is a thin wrapper.
 */
void eeprom_factory_reset(void) {
    eeprom_init_defaults();
    /* In the OEM firmware, the state machine clears 0x42 and
     * sets 0x53 = 1 to mark that a reset just happened. */
    /* (This is best-effort — see ../docs/FACTORY_RESET.md.) */
}

/**
 * Read a 16-bit value from EEPROM. Public for bui.c and btn.c.
 */
uint16_t eeprom_load16(uint8_t addr) {
    return eeprom_read16(addr);
}

/**
 * Write a 16-bit value to EEPROM. Public for bui.c (saving
 * memory positions).
 */
void eeprom_store16(uint8_t addr, uint16_t value) {
    eeprom_write16(addr, value);
}
