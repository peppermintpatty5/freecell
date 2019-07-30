#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stddef.h>

#include "freecell.h"

/**
 * Displays confirmation message on bottom line of screen and waits for user to
 * type 'y' or 'n'. The line is erased once either choice has been made. Return
 * values are non-zero for 'yes' and zero for 'no'.
 */
int confirm_yn(const char *message);

/**
 * Update the necessary parts of the display based on which components are
 * involved in the specified transfer.
 */
void update_display(struct freecell_t *f, struct transfer_t *t);

/**
 * Clears and re-prints the screen for a new game of FreeCell. If 'f' is NOT a
 * new game, then use 'update_display'.
 */
void refresh(struct freecell_t *f);

/**
 * Processes the result of a single getch().
 */
void accept_keypress(struct freecell_t *f, struct transfer_t *t);

void g_init(void);

#endif
