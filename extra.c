#include <dos.h>

#include "cascade.h"
#include "extra.h"
#include "freecell.h"
#include "graphics.h"

/**
 * Helper function that returns the number of continuously stackable cards from
 * the top of the cascade. The minimum value of 1 indicates that the top card
 * is not stackable on the card beneath it. By contrast, a value equal to
 * 'src->size' indicates that the entire cascade is a valid FreeCell stack.
 */
static size_t count_stack_streak(struct cascade_t *src)
{
	size_t i;

	for (i = src->size - 1; i > 0; i--)
	{
		if (!can_stack(src->cards[i], src->cards[i - 1]))
			break;
	}

	return src->size - i;
}

static size_t count_empty_stacks(struct freecell_t *f, struct cascade_t *src)
{
	size_t i, sum = 0;
	struct cascade_t *dst;

	for (i = 0; i < f->num_cascades; i++)
	{
		dst = f->cascades[i];
		if (src != dst && !dst->size)
			sum++;
	}

	return sum;
}

/**
 * Unloads as many cards to the freecells as needed to facilitate the transfer
 * of a column of cards, such that all progress remains completely reversable.
 * Then, it will attempt to perform 'f_transfer(f, t)' and return this result.
 * 
 * Since this is a helper function to 'auto_transfer', 't' is guaranteed to be
 * a cascade-to-cascade transfer.
 * 
 * Do NOT try to unload from empty source cascade!
 */
static int unload(struct freecell_t *f, struct transfer_t *t)
{
	struct cascade_t *src = f->cascades[t->srci],
					 *dst = f->cascades[t->dsti];
	size_t lim = count_stack_streak(src) - 1;

	t->dstsel = S_FREECELL;
	while (lim-- && (!dst->size || !can_stack(c_peek(src), c_peek(dst))))
	{
		if (!f_transfer(f, t)) /* could run out of freecell space */
			break;
		else
			update_display(f, t);

		delay(200);
	}
	t->dstsel = S_CASCADE;

	return f_transfer(f, t);
}

static int reload(struct freecell_f *f, struct transfer_t *t)
{
}

int auto_transfer(struct freecell_t *f, struct transfer_t *t)
{
	size_t i, j;
	struct cascade_t *src, *dst;

	if (t->srcsel == S_CASCADE && t->dstsel == S_CASCADE && t->srci != t->dsti)
	{
		unload(f, t);
		return 1;
	}
	else
		return f_transfer(f, t);
}