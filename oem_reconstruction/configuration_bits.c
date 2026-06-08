/**
 * @file configuration_bits.c
 * @brief BEKANT controller — recovered PIC16LF1938 __CONFIG word.
 *
 * Reconstructed from orginafirm.hex (PIC16LF1938). The CONFIG word is
 * emitted by gpdasm as `CONFIG FOSC=INTOSC` etc. at the top of the
 * disassembly; we reproduce the same settings here.
 *
 * Best-effort reconstruction — see README.md in this folder.
 */

#if defined(__XC)
    #include <xc.h>
#elif defined(HI_TECH_C)
    #include <htc.h>
#endif

// FOSC=INTOSC: Internal oscillator, I/O on RA6/RA7 (no crystal).
// WDTE=ON:     Watchdog timer always on.
// PWRTE=OFF:   Power-up timer disabled.
// MCLRE=OFF:   RA3/MCLR pin is digital I/O, not reset.
// CP=OFF, CPD=OFF: No code / data read protection.
// BOREN=ON, BORV=HI: Brown-out reset at high threshold (2.7V).
// CLKOUTEN=OFF: RA6 is I/O, not clock output.
// IESO=OFF, FCMEN=OFF: No internal/external switchover.
// WRT=ALL:     All program memory write-protected (none writable).
// VCAPEN=OFF:  No voltage regulator capacitor on RA5.
// PLLEN=OFF:   4x PLL disabled (32 MHz internal not used).
// STVREN=ON:   Stack overflow / underflow reset enabled.
// DEBUG=OFF:   In-circuit debugger pins disabled.
// LVP=OFF:     Low-voltage ICSP disabled.

// (The XC8 pragma syntax for these is shown below; once the missing
//  PIC SFR headers are available it compiles to the same bits as
//  the original firmware.)
//
// #pragma config FOSC = INTOSC, WDTE = ON, PWRTE = OFF, MCLRE = OFF
// #pragma config CP = OFF, CPD = OFF, BOREN = ON, CLKOUTEN = OFF
// #pragma config IESO = OFF, FCMEN = OFF, WRT = ALL, VCAPEN = OFF
// #pragma config PLLEN = OFF, STVREN = ON, BORV = HI, DEBUG = OFF
// #pragma config LVP = OFF
