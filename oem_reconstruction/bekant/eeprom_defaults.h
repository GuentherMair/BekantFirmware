/**
 * @file eeprom_defaults.h
 * @brief BEKANT controller — recovered EEPROM helpers.
 */
#ifndef EEPROM_DEFAULTS_H
#define EEPROM_DEFAULTS_H

#include <stdint.h>
#include <stdbool.h>

/** Write OEM default positions to EEPROM. */
void eeprom_init_defaults(void);

/** Write ivanwick-style defaults only if EEPROM is uninitialised. */
void eeprom_init_if_empty(void);

/** Get the factory-reset magic byte (0x55). */
uint8_t factory_reset_magic(void);

/** Perform a factory reset (writes OEM defaults to EEPROM). */
void eeprom_factory_reset(void);

/** Read a 16-bit value from EEPROM. */
uint16_t eeprom_load16(uint8_t addr);

/** Write a 16-bit value to EEPROM. */
void eeprom_store16(uint8_t addr, uint16_t value);

#endif /* EEPROM_DEFAULTS_H */
