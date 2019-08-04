#ifndef _EXTRA_H_
#define _EXTRA_H_

#include <stddef.h>

#include "freecell.h"

/**
 * If the player wants to move a stack between cascades in one shot, then this
 * function will attempt to automate that transfer. Otherwise, this function
 * will defer to f_transfer().
 *
 * This function will also perform the necessary calls to 'update_display'.
 */
int auto_transfer(FreeCell *f, const Transfer *t);

#endif
