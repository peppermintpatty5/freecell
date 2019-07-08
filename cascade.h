#ifndef _CASCADE_H_
#define _CASCADE_H_

/**
 * Stack data structure of cards (bytes). The stacks of cards in FreeCell are
 * often referred to as cascades.
 * 
 * This struct utilizes the zero-length array trick to be dynamically sized.
 * 
 * DO NOT use any of the respective stack functions with invalid input, such as
 * passing a null pointer, popping from an empty stack, or pushing to a full
 * stack. Segmentation faults and other catastrophes will happen.
 */
struct cascade_t
{
	size_t size;   /* The number of cards on the stack */
	char cards[0]; /* Data for the cards on the stack */
};

/**
 * Allocates memory for cascade, ensuring that the 'cards' array will have
 * capacity for 'max_size' cards (bytes).
 */
struct cascade_t *cascade_new(size_t max_size);

/**
 * Soft-clears (no overwriting) the stack by setting 'stack->size' to 0.
 */
void c_clr(struct cascade_t *stack);

/**
 * Returns the top card (highest index) of the stack.
 */
char c_peek(struct cascade_t *stack);

/**
 * Decrements stack size and returns the top card (highest index).
 */
char c_pop(struct cascade_t *stack);

/**
 * Increments stack size and puts card at the top (highest index).
 */
void c_push(struct cascade_t *stack, char card);

/**
 * Removes the nth card in the stack, shifting higher cards down the stack to
 * preserve FIFO order.
 */
char c_rm(struct cascade_t *stack, size_t index);

#endif
