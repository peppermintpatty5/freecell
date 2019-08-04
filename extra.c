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
static size_t count_stack_streak(Cascade *src)
{
	size_t i;

	for (i = src->size - 1; i > 0; i--)
	{
		if (!can_stack(src->cards[i], src->cards[i - 1]))
			break;
	}

	return src->size - i;
}

/**
 * Unloads as many cards to the freecells as needed to facilitate the transfer
 * of a column of cards, such that all progress remains completely reversable
 * via 't_reverse' + 'reload'.

 * Since this is a helper function to 'auto_transfer', 't' is guaranteed to be
 * a cascade-to-cascade transfer.
 *
 * Do NOT try to unload from an empty source cascade!
 */
static void unload(FreeCell *f, Transfer *t)
{
	Transfer T = *t;
	Cascade *src = f->cascades[t->srci],
			*dst = f->cascades[t->dsti];
	size_t limit = count_stack_streak(src) - 1;

	T.dstt = ST_FREECELL;
	while (limit-- && (!dst->size || !can_stack(c_peek(src), c_peek(dst))))
	{
		if (!f_transfer(f, &T)) /* could run out of freecell space */
			break;
		else
			update_display(f, &T);

		delay(200);
	}
}

/**
 * Reloads the destination cascade using the cards in the freecells. The
 * 'limit' parameter is provided to prevent stray cards from being reloaded,
 * and to preserve the original size of the freecells during 'auto_transfer'.
 */
static void reload(FreeCell *f, Transfer *t, size_t limit)
{
	Transfer T = *t;

	T.srci = f->freecells->size;
	T.srct = ST_FREECELL;
	while (T.srci-- > limit)
	{
		if (!f_transfer(f, &T))
			break;
		else
			update_display(f, &T);

		delay(200);
	}
}

/**
 * Attempts to locate an empty cascade. If found, the destination of 't' is
 * updated accordingly and non-zero is returned, zero otherwise.
 */
static int find_empty(FreeCell *f, Transfer *t)
{
	size_t i;

	t->dstt = ST_CASCADE; /* just to be sure */
	for (i = 0; i < f->num_cascades; i++)
	{
		if (i != t->srci && !f->cascades[i]->size)
		{
			t->dsti = i;
			return 1;
		}
	}

	return 0;
}

int auto_transfer(FreeCell *f, const Transfer *t)
{
	Transfer T = *t;
	Cascade *src, *dst;
	const size_t limit = f->freecells->size;

	if (T.srct == ST_CASCADE && T.dstt == ST_CASCADE && T.srci != T.dsti)
	{
		unload(f, &T);

		if (f_transfer(f, &T))
			update_display(f, &T);
		else
			t_reverse(&T);

		reload(f, &T, limit);

		return 1;
	}
	else
		return f_transfer(f, &T);
}
