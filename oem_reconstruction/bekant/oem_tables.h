/**
 * @file oem_tables.h
 * @brief Recovered data tables from the OEM firmware (0x1046-0x10D7).
 */
#ifndef OEM_TABLES_H
#define OEM_TABLES_H

#include <stdint.h>

/**
 * The 141-byte retlw lookup table lifted byte-for-byte from
 * disassembly 0x104B-0x10D7 of orginafirm.hex. See oem_tables.c
 * for the structural breakdown.
 */
extern const uint8_t oem_retlw_table[141];

#endif /* OEM_TABLES_H */
