#include <stdlib.h>

#include "cascade.h"

Cascade *cascade_new(size_t max_size)
{
	Cascade *stack = malloc(sizeof(Cascade) + max_size * sizeof(Card));

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

void c_clr(Cascade *stack)
{
	stack->size = 0;
}

void c_push(Cascade *stack, Card card)
{
	stack->cards[stack->size++] = card;
}

Card c_peek(Cascade *stack)
{
	return stack->cards[stack->size - 1];
}

Card c_pop(Cascade *stack)
{
	return stack->cards[--stack->size];
}

Card c_rm(Cascade *stack, size_t index)
{
	size_t i;
	Card rm = stack->cards[index];

	stack->size--;
	for (i = index; i < stack->size; i++)
		stack->cards[i] = stack->cards[i + 1];

	return rm;
}

int can_stack(Card a, Card b)
{
	return getrank(b) - getrank(a) == 1 && isblack(a) != isblack(b);
}
