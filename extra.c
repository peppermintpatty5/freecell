#include <stdlib.h>

#include <dos.h>
#include <conio.h>

#include "cascade.h"
#include "extra.h"
#include "freecell.h"
#include "graphics.h"

#define DELAY_MS (100)
#define KING_WIDTH (32)
#define KING_HEIGHT (17)

static const char KING[] =
	"\x00\x00\x00\x00\x00\x00\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A"
	"\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\xA0\xA0\xAA\xA0\xAA\xA0\xAA\x00\xA1\x00\xA1\x00\xA1\x00\xA1"
	"\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x11\x01\x11\x01\x11\x10\x10\x10\x10\x10\x10\x10\x10\x10\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0E\x00\x0E"
	"\x00\x0E\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x0E\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xEE\x00\xEE\x00\xEE\x00\x00"
	"\x0F\x00\x00\xF0\x0F\x00\x00\x00\xE0\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\xEE\x00\xEE\x00\xEE\x00\xFF\xFF\xFF\xFF\x00"
	"\xFF\xFF\xFF\x00\xEE\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\xEE\x00\xEE\x00\xEE\x00\xFF\xF0\xFF\xF0\x00\xF0\xFF\xFF\x00"
	"\xEE\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xEE"
	"\x00\xEE\x00\xEE\x00\xFF\x0F\x0F\xF0\xF0\xF0\x0F\xFF\x00\xE0\x0E\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0E\xE0\xEE\x00\xEE\x00\xEE\x00\xEE"
	"\x00\xFF\xF0\xF0\xFF\xFF\xFF\xF0\xFF\x00\xEE\x00\xEE\xEE\x00\x00\x00\x00"
	"\x00\x00\x00\x0A\x00\xE0\x0E\x0E\xEE\xE0\x0E\xEE\x00\xEE\x00\x0E\xEE\x00"
	"\xEE\x00\xE0\x00\xEE\x0E\xE0\xE0\x00\x0A\xA0\x00\x0D\x00\xAA\x00\x10\xA0"
	"\x0A\x00\x00\x0A\x00\xE0\x00\x00\xE0\x00\xE0\xEE\x0E\xE0\x0E\xE0\x00\xAA"
	"\x00\x10\x00\x0A\xA0\x00\x0D\xD0\x00\x10\x00\xAA\x00\x01\xA0\x0A\x10\x01"
	"\x01\xA0\x00\xA0\x00\xA0\x00\x0A\x00\x0A\x00\x00\xAA\x00\x10\x00\xAA\x00"
	"\x00\xDD\x00\x11\x10\x10\x00\xA0\x0A\x00\x00\xAA\x00\x11\x11\x11\xAA\xA0"
	"\xA0\xA0\x0A\x0A\x01\x01\x00\xAA\x00\x10\x00\xAA\x00\x00\xDD\x00\x11\x10"
	"\x10\x10\xA0\x00\xAA\x00\x10\x00\xAA\x00\x10\x10\xA0\xA0\xA0\xA0\xA0\xA0"
	"\x10\x10\x0A\xA0\x01\x00\xAA\x00\x00\xDD\x00\x11\x10\x10\x10\x10\xA0\xA0"
	"\x00\xAA\x00\x00\xAA\x00\x0D\xDD\xD0\xDD\x0D\xDD\xD0\xDD\x0D\x00\xAA\x00"
	"\x00\x0A\xA0\x00\x0D\xD0\x01\x10\x0A\xA0\xA0\xA0\xA0\xA0\x00\xAA\x00\x10"
	"\x00\xAA\x00\xDD\x0D\xDD\x00\xDD\x0D\xDD\xD0\x00\xA0\x01\x00\xAA\x00\x00"
	"\xD0\x00\x11\x01\xA0\xA0\x00\xD0\xA0\xA0\xA0\x00\xA0\x00\x00\xA0\x00\xD0"
	"\x00\xD0\xD0\xD0\x00\xD0\x00\x00\x00\x00\x00\xA0\x00\xA0\x00\x00\x00\x10"
	"\x10\x10\x10\x10";

static void ascii_king(int _x, int _y, int flip_h)
{
	char lo, hi;
	int x, y;
	size_t n;

	for (y = 0; y < KING_HEIGHT; y++)
	{
		gotoxy(_x, _y + y);

		for (x = 0; x < KING_WIDTH; x++)
		{
			n = flip_h ? KING_WIDTH * (y + 1) - (x + 1) : KING_WIDTH * y + x;
			lo = (unsigned char)(KING[n]) & 0xF;
			hi = (unsigned char)(KING[n]) >> 4;

			if (lo ^ hi)
			{
				textcolor(lo & 0x8 ? lo : hi);
				textbackground(lo & 0x8 ? hi : lo);
				cprintf(lo & 0x8 ? "\xDC" : "\xDF");
			}
			else
			{
				textcolor(lo);
				if (lo)
					cprintf("\xDB");
				else
					gotoxy(_x + x + 1, _y + y);
			}
		}
	}
}

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
