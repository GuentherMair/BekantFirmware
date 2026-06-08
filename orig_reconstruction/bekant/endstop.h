#ifndef ORIG_ENDSTOP_H
#define ORIG_ENDSTOP_H

#include <stdint.h>
#include <stdbool.h>

/**
 * The endstop detector — recovered from the OEM firmware.
 *
 * The OEM firmware watches the leg encoders and, when a desk leg
 * stops moving while the BCTRL is still commanding motion (i.e. it
 * has hit the physical endstop), issues BCMD_STOP (0xfc) to drop
 * the relay. This module encapsulates that logic and exposes a
 * single callback that bctrl.c calls once per scheduler tick.
 *
 * The ivanwick project (which we are extending) has no equivalent
 * — see docs/ENDSTOP_ANALYSIS.md for the root-cause analysis.
 */

void endstop_init(void);
void endstop_timer(void);

/* Called by bctrl.c every time the leg positions are reported. The
 * detector decides whether to force a stop. */
extern void (*endstop_report)(int16_t pos);

/* Convenience: the callback bctrl.c can register to forward its
 * position report to the endstop detector.  Defined in endstop.c. */
void bctrl_stop_if_at_endstop_cb(int16_t pos);

#endif /* ORIG_ENDSTOP_H */
