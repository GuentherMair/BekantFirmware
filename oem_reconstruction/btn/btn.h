#ifndef BTN_H
#define BTN_H

#include <stdint.h>

/**
 * Recovered from the OEM firmware's button state machine at 0x05c0.
 *
 * The OEM firmware's state register (RAM 0x4d) cycles through values
 * 0x00..0x07. The mapping to user-visible gestures is:
 *
 *   0x01  - both buttons pressed (save mode)
 *   0x02  - save-in-progress / confirmation counter running
 *   0x03  - move-up gesture acknowledged
 *   0x05  - approach target encoder value
 *   0x07  - endstop-detected: send BCMD_STOP (0xfc) and reset
 */
typedef enum {
    INPUT_IDLE,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_MEM_UP,     /* OEM firmware uses the same name; "memory up" */
    INPUT_MEM_DOWN,
    INPUT_SAVE,
} INPUT_t;

void btn_init(void);
void btn_timer(void);

/* Hook for the BUI module to receive debounced button gestures.
 * Set this before calling btn_init(). */
extern void (*btn_report_gesture)(INPUT_t gesture);

#endif /* BTN_H */
