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

/**
 * Ugly recursive function that should work.
 */
static int z(FreeCell *f, const Transfer *t, Transfer *last, const size_t n)
{
	Transfer T = *t;
	size_t i, _n;

	/* Move up to 'n - 1' card to the freecells */
	T.dstt = ST_FREECELL;
	for (i = 0; i < n - 1; i++)
	{
		if (f_transfer(f, &T))
		{
			update_display(f, &T);
			delay(200);
		}
		else
			break;
	}
	_n = n - i;
	T.dstt = ST_CASCADE;

	if (_n > 1) /* There are still cards that need to be moved */
	{
		if (find_empty(f, &T))
		{
			f_transfer(f, &T);
			update_display(f, &T);
			delay(200);
			T.srct = ST_FREECELL;
			T.srci = f->freecells->size - 1;
			for (i = 0; i < n - _n; i++)
			{
				f_transfer(f, &T);
				update_display(f, &T);
				delay(200);
				T.srci--;
			}
			T.srct = ST_CASCADE;

			if (last)
			{
				T.srci = last->dsti;
				z(f, &T, NULL, f->cascades[T.srci]->size);
			}

			T.srci = t->srci;

			return z(f, t, &T, _n - 1);
		}
		else /* No more options */
		{
			t_reverse(&T);
			T.srct = ST_FREECELL;
			T.srci = f->freecells->size - 1;
			for (i = 0; i < n - _n; i++)
			{
				f_transfer(f, &T);
				update_display(f, &T);
				delay(200);
				T.srci--;
			}

			return 0;
		}
	}
	else /* Move the final card to the destination */
	{
		f_transfer(f, &T);
		update_display(f, &T);
		delay(200);
		T.srct = ST_FREECELL;
		T.srci = f->freecells->size - 1;
		for (i = 0; i < n - _n; i++)
		{
			f_transfer(f, &T);
			update_display(f, &T);
			delay(200);
			T.srci--;
		}

		if (last)
		{
			t_reverse(last);
			last->dsti = t->dsti;
			z(f, last, NULL, n);
		}

		return 1;
	}
}

int auto_transfer(FreeCell *f, const Transfer *t)
{
	size_t i;

	if (t->srct == ST_CASCADE && t->dstt == ST_CASCADE && t->srci != t->dsti)
	{
		i = auto_card_count(f->cascades[t->srci], f->cascades[t->dsti]);

		return i ? z(f, t, NULL, i) : 0;
	}
	else
		return f_transfer(f, t);
}
