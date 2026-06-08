/* Test SFR stubs for syntax checking with gcc. */
#ifndef TEST_SFR_STUBS_H
#define TEST_SFR_STUBS_H
#include <stdint.h>
#include <stdbool.h>
typedef uint8_t uint8;
typedef int8_t  int8;
typedef uint16_t uint16;
typedef int16_t  int16;
typedef uint32_t uint32;
typedef int32_t  int32;
#define _XTAL_FREQ 16000000
extern uint8_t __ANSELB, __TRISB, __LATC, __TRISC, __PORTC, __PORTB, __TMR4;
extern uint8_t __SPBRGL, __SPBRGH, __RCREG, __TXREG, __TRISC6, __TRISC7;
extern uint8_t __EEADR, __EEDATA, __EECON1, __EECON2;
extern uint8_t __T2CON, __PR2, __PIE1, __PIR1, __T4CON, __PR4, __PIE3, __PIR3;
extern uint8_t __OSCCON, __RCSTA, __TXSTA, __BAUDCON, __INTCON;
typedef union { struct { unsigned DOWN:1; unsigned UP:1; unsigned:6; }; uint8_t value; } PORTBbits_t;
extern PORTBbits_t __PORTBbits;
typedef union { struct { unsigned:6; unsigned TRISC6:1; unsigned TRISC7:1; }; uint8_t value; } TRISCbits_t;
extern TRISCbits_t __TRISCbits;
typedef struct { unsigned T2CKPS:2; unsigned TMR2ON:1; unsigned T2OUTPS:4; } T2CONbits_t;
typedef struct { uint8_t PR2; } PR2bits_t;
typedef struct { unsigned TMR2IE:1; unsigned RCIE:1; } PIE1bits_t;
typedef struct { unsigned TMR2IF:1; unsigned RCIF:1; unsigned TXIF:1; } PIR1bits_t;
typedef struct { unsigned T4CKPS:2; unsigned TMR4ON:1; unsigned T4OUTPS:4; } T4CONbits_t;
typedef struct { uint8_t PR4; } PR4bits_t;
typedef struct { unsigned TMR4IE:1; } PIE3bits_t;
typedef struct { unsigned TMR4IF:1; } PIR3bits_t;
typedef struct { unsigned RD:1; unsigned WR:1; unsigned WREN:1; unsigned WRERR:1; unsigned FREE:1; unsigned LWLO:1; unsigned CFGS:1; unsigned EEPGD:1; } EECON1bits_t;
typedef struct { unsigned SCS:2; unsigned IRCF:4; } OSCCONbits_t;
typedef struct { unsigned SPEN:1; unsigned CREN:1; unsigned ADDEN:1; } RCSTAbits_t;
typedef struct { unsigned SYNC:1; unsigned BRGH:1; unsigned SENDB:1; unsigned TXEN:1; } TXSTAbits_t;
typedef struct { unsigned BRG16:1; unsigned WUE:1; unsigned ABDEN:1; unsigned SCKP:1; } BAUDCONbits_t;
typedef struct { unsigned GIE:1; unsigned PEIE:1; } INTCONbits_t;
extern T2CONbits_t __T2CONbits;
extern PR2bits_t __PR2bits;
extern PIE1bits_t __PIE1bits;
extern PIR1bits_t __PIR1bits;
extern T4CONbits_t __T4CONbits;
extern PR4bits_t __PR4bits;
extern PIE3bits_t __PIE3bits;
extern PIR3bits_t __PIR3bits;
extern EECON1bits_t __EECON1bits;
extern OSCCONbits_t __OSCCONbits;
extern RCSTAbits_t __RCSTAbits;
extern TXSTAbits_t __TXSTAbits;
extern BAUDCONbits_t __BAUDCONbits;
extern INTCONbits_t __INTCONbits;
#define ANSELB __ANSELB
#define TRISB  __TRISB
#define LATC   __LATC
#define TRISC  __TRISC
#define TMR4   __TMR4
#define SPBRGL __SPBRGL
#define SPBRGH __SPBRGH
#define RCREG  __RCREG
#define TXREG  __TXREG
#define EEADR  __EEADR
#define EEDATA __EEDATA
#define EECON1 __EECON1
#define EECON2 __EECON2
#define T2CON  __T2CON
#define PR2    __PR2
#define PIE1   __PIE1
#define PIR1   __PIR1
#define T4CON  __T4CON
#define PR4    __PR4
#define PIE3   __PIE3
#define PIR3   __PIR3
#define OSCCON __OSCCON
#define RCSTA  __RCSTA
#define TXSTA  __TXSTA
#define BAUDCON __BAUDCON
#define INTCON __INTCON
#define PORTB __PORTBbits.value
#define PORTBbits __PORTBbits
#define TRISCbits __TRISCbits
#define T2CONbits __T2CONbits
#define PR2bits __PR2bits
#define PIE1bits __PIE1bits
#define PIR1bits __PIR1bits
#define T4CONbits __T4CONbits
#define PR4bits __PR4bits
#define PIE3bits __PIE3bits
#define PIR3bits __PIR3bits
#define EECON1bits __EECON1bits
#define OSCCONbits __OSCCONbits
#define RCSTAbits __RCSTAbits
#define TXSTAbits __TXSTAbits
#define BAUDCONbits __BAUDCONbits
#define INTCONbits __INTCONbits
#define CLRWDT() do {} while (0)
#define __EEPROM_DATA(...)
#define __eeprom
#define NOP() do {} while (0)
#endif
