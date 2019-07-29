#ifndef _MAIN_H_
#define _MAIN_H_

#include "freecell.h"

#define hidecursor gotoxy(1, 1)

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

void refresh(void);

#endif
