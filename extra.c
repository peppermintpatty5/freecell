#include "extra.h"

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
