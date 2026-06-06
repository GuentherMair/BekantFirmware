/* Test SFR stubs — only used for gcc -fsyntax-only checks, not for real XC8 builds. */
#ifndef TEST_SFR_STUBS_H
#define TEST_SFR_STUBS_H

#include <stdint.h>
#include <stdbool.h>

/* Use extern declarations; the actual storage is in test_sfr_stubs.c.
 * The XC8 build system normally provides these as addressable memory
 * locations. For a syntax-only test we just need a valid lvalue. */

extern uint8_t _ANSELB;
extern uint8_t _TRISB;
extern uint8_t _LATC;
extern uint8_t _TRISC;
extern uint8_t _PORTC;
extern uint8_t _PORTB;
extern uint8_t _TMR4;
extern uint8_t _SPBRGL;
extern uint8_t _SPBRGH;
extern uint8_t _RCREG;
extern uint8_t _TXREG;
extern uint8_t _TRISC6;
extern uint8_t _TRISC7;

#define ANSELB _ANSELB
#define TRISB  _TRISB
#define LATC   _LATC
#define TRISC  _TRISC
#define TMR4   _TMR4
#define SPBRGL _SPBRGL
#define SPBRGH _SPBRGH
#define RCREG  _RCREG
#define TXREG  _TXREG

/* PORTB — used as a struct of bit fields in btn.c. We model it as a
 * single byte plus a union to allow bitfield access. */
typedef union {
    struct {
        unsigned DOWN : 1;
        unsigned UP   : 1;
        unsigned      : 6;
    };
    uint8_t value;
} PORTBbits_t;

extern PORTBbits_t _PORTBbits;
#define PORTB _PORTBbits.value
#define PORTBbits _PORTBbits

/* PORTC bits */
typedef union {
    struct {
        unsigned      : 6;
        unsigned TRISC6 : 1;
        unsigned TRISC7 : 1;
    };
    uint8_t value;
} TRISCbits_t;
extern TRISCbits_t _TRISCbits;
#define TRISCbits _TRISCbits

/* Timer2 */
typedef struct {
    unsigned T2CKPS  : 2;
    unsigned TMR2ON  : 1;
    unsigned T2OUTPS : 4;
} T2CONbits_t;
typedef struct {
    unsigned PR2 : 8;
} PR2bits_t;
typedef struct {
    unsigned TMR2IE : 1;
    unsigned RCIE   : 1;
} PIE1bits_t;
typedef struct {
    unsigned TMR2IF : 1;
    unsigned RCIF   : 1;
    unsigned TXIF   : 1;
} PIR1bits_t;

extern T2CONbits_t  _T2CONbits;
extern PR2bits_t    _PR2bits;
extern PIE1bits_t   _PIE1bits;
extern PIR1bits_t   _PIR1bits;
#define T2CONbits _T2CONbits
#define PR2bits   _PR2bits
#define PIE1bits  _PIE1bits
#define PIR1bits  _PIR1bits

/* Timer4 */
typedef struct {
    unsigned T4CKPS  : 2;
    unsigned TMR4ON  : 1;
    unsigned T4OUTPS : 4;
} T4CONbits_t;
typedef struct {
    unsigned PR4 : 8;
} PR4bits_t;
typedef struct {
    unsigned TMR4IE : 1;
} PIE3bits_t;
typedef struct {
    unsigned TMR4IF : 1;
} PIR3bits_t;

extern T4CONbits_t  _T4CONbits;
extern PR4bits_t    _PR4bits;
extern PIE3bits_t   _PIE3bits;
extern PIR3bits_t   _PIR3bits;
#define T4CONbits _T4CONbits
#define PR4bits   _PR4bits
#define PIE3bits  _PIE3bits
#define PIR3bits  _PIR3bits

/* OSCCON */
typedef struct {
    unsigned SCS   : 2;
    unsigned IRCF  : 4;
} OSCCONbits_t;
extern OSCCONbits_t _OSCCONbits;
#define OSCCONbits _OSCCONbits

/* USART */
typedef struct {
    unsigned SPEN   : 1;
    unsigned CREN   : 1;
    unsigned ADDEN  : 1;
} RCSTAbits_t;
typedef struct {
    unsigned SYNC   : 1;
    unsigned BRGH   : 1;
    unsigned SENDB  : 1;
    unsigned TXEN   : 1;
} TXSTAbits_t;
typedef struct {
    unsigned BRG16  : 1;
    unsigned WUE    : 1;
    unsigned ABDEN  : 1;
    unsigned SCKP   : 1;
} BAUDCONbits_t;

extern RCSTAbits_t   _RCSTAbits;
extern TXSTAbits_t   _TXSTAbits;
extern BAUDCONbits_t _BAUDCONbits;
#define RCSTAbits   _RCSTAbits
#define TXSTAbits   _TXSTAbits
#define BAUDCONbits _BAUDCONbits

/* INTCON */
typedef struct {
    unsigned GIE  : 1;
    unsigned PEIE : 1;
} INTCONbits_t;
extern INTCONbits_t _INTCONbits;
#define INTCONbits _INTCONbits

/* Helper macros */
#define CLRWDT()              do {} while (0)
#define __EEPROM_DATA(...)    /* no-op for syntax check */
#define __eeprom              /* no-op qualifier */

#endif
