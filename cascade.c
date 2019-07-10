#include <stdio.h>
#include <stdlib.h>

#include "cascade.h"

struct cascade_t *cascade_new(size_t max_size)
{
	struct cascade_t *stack =
		malloc(sizeof(struct cascade_t) + max_size * sizeof(char));

	if (stack)
	{
		stack->size = 0;
	}
	else
	{
		perror("malloc failed in cascade_new\n");
		exit(EXIT_FAILURE);
	}

	return stack;
}

void c_clr(struct cascade_t *stack)
{
	stack->size = 0;
}

char c_peek(struct cascade_t *stack)
{
	return stack->cards[stack->size - 1];
}

char c_pop(struct cascade_t *stack)
{
	return stack->cards[--stack->size];
}

void c_push(struct cascade_t *stack, char card)
{
	stack->cards[stack->size++] = card;
}

char c_rm(struct cascade_t *stack, size_t index)
{
	size_t i;
	char rm = stack->cards[index];

	stack->size--;
	for (i = index; i < stack->size; i++)
		stack->cards[i] = stack->cards[i + 1];

	return rm;
}
