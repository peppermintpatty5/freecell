#include <stdlib.h>

#include <dos.h>
#include <conio.h>

#include "cascade.h"
#include "extra.h"
#include "freecell.h"
#include "graphics.h"

typedef struct
{
	size_t size, data[0];
} Something;

static Something *indexset_new(size_t max_size)
{
	Something *hmm = malloc(sizeof(Something) + max_size * sizeof(size_t));

	if (hmm)
	{
		hmm->size = 0;
	}
	else
	{
		perror("malloc failed in SOMETHING_new\n");
		exit(EXIT_FAILURE);
	}

	return hmm;
}

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

/**
 * Returns a hmm containing the indices of all the empty cascades.
 */
static Something *find_empty(FreeCell *f)
{
	size_t i;
	Something *hmm = indexset_new(f->num_cascades);

	for (i = 0; i < f->num_cascades; i++)
	{
		if (!f->cascades[i]->size)
			hmm->data[hmm->size++] = i;
	}

	return hmm;
}

/**
 * Recursive function for the transfer of multiple cards to an empty cascade.
 * Returns an updated quota for how many cards still need to be moved.
 */
static size_t xyz(FreeCell *f, Something *hmm, size_t srci, size_t quota)
{
	/* TODO: use the FreeCells, add upper limit parameter */
	size_t tmpi;
	Transfer T = {0, 0, ST_CASCADE, ST_CASCADE};

	T.srci = srci;
	T.dsti = hmm->data[hmm->size - 1];

	if (hmm->size == 1) /* No temporary columns */
	{
		f_transfer(f, &T);
		update_display(f, &T);
		delay(200);
		quota--;

		return quota;
	}
	else
	{
		/* Source to temporary */
		hmm->size--;
		quota = xyz(f, hmm, srci, quota);

		/* Source to destination */
		tmpi = hmm->data[hmm->size - 1];
		hmm->data[hmm->size - 1] = T.dsti;
		if (quota)
			quota = xyz(f, hmm, srci, quota);

		/* Temporary to destination */
		xyz(f, hmm, tmpi, f->cascades[tmpi]->size);
		hmm->data[hmm->size - 1] = tmpi;
		hmm->size++;

		return quota;
	}
}

int auto_transfer(FreeCell *f, const Transfer *t)
{
	int result;
	size_t i, quota;
	Something *hmm;
	Transfer T;
	Cascade *src, *dst;

	if (t->srct == ST_CASCADE && t->dstt == ST_CASCADE && t->srci != t->dsti)
	{
		src = f->cascades[t->srci];
		dst = f->cascades[t->dsti];
		quota = auto_card_count(src, dst) - 1;
		hmm = find_empty(f);

		if (quota)
		{
			i = hmm->size; /* backup */
			while (hmm->size)
			{
				if (quota)
					quota = xyz(f, hmm, t->srci, quota);
				hmm->size--; /* achieves the effect of removal from set */
			}
			hmm->size = i; /* restore */
		}

		result = f_transfer(f, t);
		update_display(f, t);

		if (result)
		{
			T = *t;
			for (i = 0; i < hmm->size; i++)
			{
				T.srci = hmm->data[i];
				if (f->cascades[T.srci]->size) /* check against empty */
					auto_transfer(f, &T);
			}
		}

		free(hmm);

		return result;
	}
	else
		return f_transfer(f, t);
}
