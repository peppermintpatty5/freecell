#include "extra.h"

#if 0
size_t count_stack_streak(struct cascade_t *src)
{
	size_t i;

	for (i = src->size - 1; i > 0; i--)
	{
		if (!can_stack(src->cards[i], src->cards[i - 1]))
			break;
	}

	return src->size - i;
}

void idkman(size_t srci, size_t dsti, size_t count, int delay_ms)
{
	size_t i;

	for (i = 0; i < count - 1; i++)
	{
		cascade_to_freecell(srci);
		hidecursor();
		delay(delay_ms);
	}
	cascade_to_cascade(srci, dsti);
	for (i = 0; i < count - 1; i++)
	{
		delay(delay_ms);
		freecell_to_cascade(freecells->size - 1, dsti);
		hidecursor();
	}
}

size_t count_empty_cascades(size_t dsti)
{
	size_t i, j;

	for (i = 0, j = 0; i < NUM_CASCADES; i++)
	{
		if (!cascades[i]->size)
			j++;
	}

	return j - !cascades[dsti]->size;
}

int cascade_to_cascade_m(int srci, int dsti, int delay_ms)
{
	struct cascade_t *src = cascades[srci],
					 *dst = cascades[dsti];
	size_t count = count_stack_streak(src),
		   num_empty = count_empty_cascades(dsti),
		   num_free = NUM_FREECELLS - freecells->size,
		   i;
	/* Determine how many cards need to be moved, stored in 'count' */
	if (dst->size)
	{
		while (count && !can_stack(src->cards[src->size - count], c_peek(dst)))
			count--;
	}
	else if (!num_empty && count > num_free + 1)
	{
		/* Special case where 'dst' is the only empty cascade */
		count = num_free + 1;
	}
	/* Do NOT try to move 0 cards - unsigned underflow! */
	if (count)
	{
		/* Transfer can be done all in one shot */
		if (count <= num_free + 1)
		{
			idkman(srci, dsti, count, delay_ms);

			return count;
		}
		/* Transfer must be done recursively with an empty cascade */
		else
		{
			/* Try to locate an empty cascade */
			for (i = 0; i < NUM_CASCADES; i++)
			{
				if (i != dsti && !cascades[i]->size)
					break;
			}
			if (i < NUM_CASCADES)
			{
				cascade_to_cascade_m(srci, i, delay_ms);
				if (cascade_to_cascade_m(srci, dsti, delay_ms))
				{
					cascade_to_cascade_m(i, dsti, delay_ms);
					return count;
				}
				/* Out of luck */
				else
					return 0;
			}
			else
				return 0;
		}
	}
	else
		return 0;
}
#endif
