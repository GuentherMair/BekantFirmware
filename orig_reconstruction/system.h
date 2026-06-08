#ifndef SYSTEM_H
#define SYSTEM_H

/* System frequency.  Recovered from the disassembly: 16 MHz HFINTOSC. */
#ifndef _XTAL_FREQ
#define _XTAL_FREQ  16000000
#endif

void ConfigureOscillator(void);

#endif /* SYSTEM_H */
