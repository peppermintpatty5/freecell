#ifndef _EXTRA_H_
#define _EXTRA_H_

#include <stddef.h>

#include "cascade.h"

#if 0
/**
 * Helper function that returns the number of continuously stackable cards from
 * the top of the cascade. The minimum value of 1 indicates that the top card
 * is not stackable on the card beneath it. By contrast, a value equal to
 * 'src->size' indicates that the entire cascade is a valid FreeCell stack.
 */
size_t count_stack_streak(struct cascade_t *src);

void idkman(size_t srci, size_t dsti, size_t count, int delay_ms);

/**
 * Helper function that returns the number of cascades, other than the cascade
 * specified, that are empty.
 */
size_t count_empty_cascades(size_t dsti);

/**
 * A (probably) recursive function that moves multiple cards from one stack to
 * another.
 * 
 * Destination index may refer to empty cascade, while source index may not.
 * 
 * Also does cool animation if you want to see it.
 */
int cascade_to_cascade_m(int srci, int dsti, int delay_ms);

#endif

#endif
