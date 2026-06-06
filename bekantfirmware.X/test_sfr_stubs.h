/* Test SFR stubs for syntax-only checks of the compound project. */
#ifndef TEST_SFR_STUBS_H
#define TEST_SFR_STUBS_H

#include <stdint.h>
#include <stdbool.h>

/* When BEKANT_TEST_SYNTAX is set, the project .c files use these
 * stubs instead of the real PIC SFR headers (which require the
 * XC8 compiler). */

/* Make `#include <xc.h>` and `#include <pic.h>` resolve to this
 * header so the rest of the project compiles for syntax checking. */

/* Use addressable globals (declared in test_sfr_stubs.c). */
extern uint8_t _ANSELB;
extern uint8_t _TRISB;
extern uint8_t _LATC;
extern uint8_t _TRISCscalar;
extern uint8_t _PORTC;
extern uint8_t _PORTB;
extern uint8_t _TMR4;
extern uint8_t _SPBRGL;
extern uint8_t _SPBRGH;
extern uint8_t _RCREG;
extern uint8_t _TXREG;

#define ANSELB _ANSELB
#define TRISB  _TRISB
#define LATC   _LATC
#define TRISC  _TRISCscalar
#define TMR4   _TMR4
#define SPBRGL _SPBRGL
#define SPBRGH _SPBRGH
#define RCREG  _RCREG
#define TXREG  _TXREG

typedef union {
    struct {
        unsigned DOWN : 1;
        unsigned UP   : 1;
        unsigned      : 6;
    };
    uint8_t value;
} PORTBbits_t;

extern PORTBbits_t _PORTBbits;
#define PORTBbits _PORTBbits
/* Don't #define PORTB — btn.c declares a local variable of that name. */

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
#undef TRISC
#define TRISC (*(uint8_t *)&_TRISCbits)

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

typedef struct {
    unsigned T4CKPS  : 2;
    unsigned TMR4ON  : 1;
    unsigned T4OUTPS : 4;
} T4CONbits_t;
typedef struct {
    uint8_t value;
} PR4bits_t;
extern uint8_t PR4reg_storage;
#define PR4 PR4reg_storage
#define PR4bits (*(PR4bits_t *)&PR4reg_storage)
typedef struct {
    unsigned TMR4IE : 1;
} PIE3bits_t;
typedef struct {
    unsigned TMR4IF : 1;
} PIR3bits_t;

extern T4CONbits_t  _T4CONbits;
extern PIE3bits_t   _PIE3bits;
extern PIR3bits_t   _PIR3bits;
#define T4CONbits _T4CONbits
#define PIE3bits  _PIE3bits
#define PIR3bits  _PIR3bits

typedef struct {
    unsigned SCS   : 2;
    unsigned IRCF  : 4;
} OSCCONbits_t;
extern OSCCONbits_t _OSCCONbits;
#define OSCCONbits _OSCCONbits

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

typedef struct {
    unsigned SWDTEN : 1;
    unsigned WDTPS  : 4;
} WDTCONbits_t;
extern WDTCONbits_t _WDTCONbits;
#define WDTCONbits _WDTCONbits

typedef struct {
    unsigned PS      : 3;
    unsigned PSA     : 1;
    unsigned TMR0SE  : 1;
    unsigned TMR0CS  : 1;
    unsigned INTEDG  : 1;
    unsigned nWPUEN  : 1;
} OPTION_REGbits_t;
extern OPTION_REGbits_t _OPTION_REGbits;
#define OPTION_REGbits _OPTION_REGbits

typedef struct {
    unsigned GIE  : 1;
    unsigned PEIE : 1;
} INTCONbits_t;
extern INTCONbits_t _INTCONbits;
#define INTCONbits _INTCONbits

#define CLRWDT()              do {} while (0)
#define __EEPROM_DATA(...)    /* no-op for syntax check */
#define __eeprom              /* no-op qualifier */

#endif
