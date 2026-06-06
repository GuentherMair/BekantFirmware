/*
 * btn.h — Button gesture input
 *
 * ---------------------------------------------------------------------------
 * Both gesture variants are supported at the same time:
 *
 *   1) "Combo-press" (from ivanwick)
 *        hold  △   + tap ▽  → INPUT_MEM_UP   (move to upper memory)
 *        hold  ▽   + tap △  → INPUT_MEM_DOWN (move to lower memory)
 *
 *   2) "Double-tap" (from DieSteinhose)
 *        tap-tap  △   within 500 ms  → INPUT_MEM_UP
 *        tap-tap  ▽   within 500 ms  → INPUT_MEM_DOWN
 *
 * The two encodings are detected in parallel and both fire the same
 * INPUT_MEM_UP / INPUT_MEM_DOWN event.  The downstream BUI module does
 * not (and should not) care which one the user used.  A user can mix
 * and match — e.g. do a combo-press one day and a double-tap the next.
 *
 * ---------------------------------------------------------------------------
 * See ../USAGE.md for end-user documentation.
 * See ../../docs/COMPARISON.md for how this differs from each upstream.
 * ---------------------------------------------------------------------------
 */

#ifndef BTN_H
#define BTN_H

typedef enum {
    INPUT_IDLE,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_MEM_UP,
    INPUT_MEM_DOWN,
    INPUT_SAVE,
} INPUT_t;

void btn_init(void);
void btn_timer(void);

/* Set this to receive debounced button gestures.  Called from the ISR
 * (Timer2 @ 20 Hz) and from the BUI module via the bctrl wiring. */
extern void (*btn_report_gesture)(INPUT_t gesture);

#endif /* BTN_H */
