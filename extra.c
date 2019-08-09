#include <dos.h>
#include <conio.h>

#include "cascade.h"
#include "extra.h"
#include "freecell.h"
#include "graphics.h"

static size_t auto_card_count(Cascade *src, Cascade *dst)
{
	Card b;
	size_t i = src->size;

	while (--i && can_stack(src->cards[i], src->cards[i - 1]))
		continue;

	if (dst->size)
	{
		b = c_peek(dst);
		while (!can_stack(src->cards[i], b) && ++i < src->size)
			continue;
	}

	return src->size - i;
}

static int find_empty(FreeCell *f, Transfer *t)
{
	size_t i;

	for (i = 0; i < f->num_cascades; i++)
	{
		if (t->dsti != i && !f->cascades[i]->size)
		{
			t->dsti = i;
			return 1;
		}
	}

	return 0;
}

static size_t count_empty(FreeCell *f)
{
	size_t i, sum;

	for (i = sum = 0; i < f->num_cascades; i++)
	{
		if (!f->cascades[i]->size)
			sum++;
	}

	return sum;
}

static int base(FreeCell *f, const Transfer *t, int level)
{
	/* TODO: use the FreeCells, add upper limit parameter */
	Transfer T;

	if (t->dstt == ST_NONE)
		return 0;

	if (level == 0)
	{
		f_transfer(f, t);
		update_display(f, t);
		delay(200);
	}
	else
	{
		T = *t;
		find_empty(f, &T);

		base(f, &T, level - 1);
		base(f, t, level - 1);

		t_reverse(&T);
		T.dsti = t->dsti;
		base(f, &T, level - 1);
	}

	return 1;
}

int auto_transfer(FreeCell *f, const Transfer *t)
{
	size_t i;
	Transfer T;

	if (t->srct == ST_CASCADE && t->dstt == ST_CASCADE && t->srci != t->dsti)
	{
		T = *t;
		i = count_empty(f);
		while (i--)
		{
			find_empty(f, &T); /* FIXME: This is technically O(n^2) */
			base(f, &T, i);
		}
	}
	else
		return f_transfer(f, t);
}
