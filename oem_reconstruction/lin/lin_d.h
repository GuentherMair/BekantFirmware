#ifndef LIN_D_H
#define LIN_D_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t lin_id;
extern uint8_t lin_data_count;
extern uint8_t *lin_data;

/* Recovered from the disassembly: this is the LIN frame-finish callback
 * that bctrl registers to receive the "frame done" event. */
typedef struct {
    unsigned L_STATUS_BUSY : 1;
    /* Other status bits are not relevant for the recovered flow. */
} lin_flags_t;
extern volatile lin_flags_t lin_flags;

void lin_init_hw(void);
void lin_txrx_daemon(void);
void lin_tx_frame(void);
void lin_rx_frame(void);
void lin_reset_frame(void);

/* Set by bctrl_init() to point at bctrl_rx_lin. */
extern void (*lin_frame_finish)(void);

#endif /* LIN_D_H */
