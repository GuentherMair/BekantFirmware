#ifndef BUI_H
#define BUI_H

#include <stdint.h>
#include <stdbool.h>
#include "../btn/btn.h"

void bui_init(void);
void bui_set_pos(int16_t pos);
void bui_input(INPUT_t input);

/* OEM firmware has a factory-reset gesture (hold both buttons 10 s).
 * The reconstruction defers the implementation to bui.c. */
void bui_factory_reset(void);

#endif /* BUI_H */
