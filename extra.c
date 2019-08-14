#include <stdlib.h>

#include <dos.h>
#include <conio.h>

#include "cascade.h"
#include "extra.h"
#include "freecell.h"
#include "graphics.h"

#define DELAY_MS (100)

typedef struct
{
	size_t size;
	size_t data[0];
} IndexSet;

static IndexSet *indexset_new(size_t max_size)
{
	IndexSet *set = malloc(sizeof(IndexSet) + max_size * sizeof(size_t));

	if (set)
	{
		set->size = 0;
	}
	else
	{
		perror("malloc failed in SOMETHING_new\n");
		exit(EXIT_FAILURE);
	}

	return set;
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
 * Returns a set containing the indices of all the empty cascades.
 */
static IndexSet *find_empty(FreeCell *f)
{
	size_t i;
	IndexSet *set = indexset_new(f->num_cascades);

	for (i = 0; i < f->num_cascades; i++)
	{
		if (!f->cascades[i]->size)
			set->data[set->size++] = i;
	}

	return set;
}

/**
 * Recursive function for the transfer of multiple cards to an empty cascade.
 * Returns an updated quota for how many cards still need to be moved.
 */
static size_t xyz(FreeCell *f, IndexSet *set, size_t srci, size_t quota)
{
	size_t tmpi;
	Transfer T = {0, 0, ST_CASCADE, ST_CASCADE};

	T.srci = srci;
	T.dsti = set->data[set->size - 1];

	if (set->size == 1) /* No temporary columns */
	{
		f_transfer(f, &T);
		update_display(f, &T);
		delay(DELAY_MS);
		quota--;

		return quota;
	}
	else
	{
		/* Source to temporary */
		set->size--;
		quota = xyz(f, set, srci, quota);

		/* Source to destination */
		tmpi = set->data[set->size - 1];
		set->data[set->size - 1] = T.dsti;
		if (quota)
			quota = xyz(f, set, srci, quota);

		/* Temporary to destination */
		xyz(f, set, tmpi, f->cascades[tmpi]->size);
		set->data[set->size - 1] = tmpi;
		set->size++;

		return quota;
	}
}

static size_t log_2(size_t n)
{
	size_t i = 0;

	if (!n) /* undefined */
		return 0;

	n--;
	while (n)
	{
		i++;
		n >>= 1;
	}

	return i;
}

int auto_transfer(FreeCell *f, const Transfer *t)
{
	int result;
	size_t i, quota;
	IndexSet *set;
	Transfer T;
	Cascade *src, *dst;

	if (t->srct == ST_CASCADE && t->dstt == ST_CASCADE && t->srci != t->dsti)
	{
		src = f->cascades[t->srci];
		dst = f->cascades[t->dsti];
		quota = auto_card_count(src, dst);
		set = find_empty(f);

		if (quota)
		{
			/* Prevents this function from taking forever using math */
			i = log_2(quota);
			if (set->size > i)
				set->size = i;

			quota--;
			i = set->size; /* backup */
			while (set->size)
			{
				if (quota)
					quota = xyz(f, set, t->srci, quota);
				else
					break;
				set->size--; /* achieves the effect of removal from set */
			}
			set->size = i; /* restore */
		}
		else
			return 0;

		result = f_transfer(f, t);
		update_display(f, t);
		delay(DELAY_MS);

		T = *t;
		if (!result)
			T.dsti = T.srci;

		for (i = 0; i < set->size; i++)
		{
			T.srci = set->data[i];
			if (f->cascades[T.srci]->size) /* check against empty */
				auto_transfer(f, &T);
		}

		free(set);

		return result;
	}
	else
		return f_transfer(f, t);
}
