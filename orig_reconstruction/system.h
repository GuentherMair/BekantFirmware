#ifndef SYSTEM_H
#define SYSTEM_H

#include "test_sfr_stubs.h"

/* System frequency.  Recovered from the disassembly: 16 MHz HFINTOSC. */
#define _XTAL_FREQ  16000000

void ConfigureOscillator(void);

#endif /* SYSTEM_H */
