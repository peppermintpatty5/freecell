#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "conio.h"
#include "card.h"
#include "dos.h"
#include "cascade.h"
#include "freecell.h"

void deal(struct freecell_t *f)
{
	size_t i, j, r, j_ = 0;
	char deck[NUM_CARDS], c;

	for (i = 0; i < NUM_CARDS; i++)
		deck[i] = i;

	for (i = 0; i < f->num_decks; i++)
	{
		/* shuffle */
		for (j = NUM_CARDS - 1; j > 0; j--)
		{
			r = rand() % j;
			c = deck[r];
			deck[r] = deck[j];
			deck[j] = c;
		}
		/* deal */
		for (j = 0; j < NUM_CARDS; j++)
			c_push(f->cascades[(j + j_) % f->num_cascades], deck[j]);
		j_ = j % f->num_cascades;
	}
}

void newgame(struct freecell_t *f, enum game_types gt)
{
	size_t i;

	switch (gt)
	{
	case SINGLE_DECK:
		f->num_decks = 1;
		f->num_cascades = 8;
		f->num_freecells = 4;
		break;
	case DOUBLE_DECK:
		f->num_decks = 2;
		f->num_cascades = 10;
		f->num_freecells = 8;
		break;
	}

	for (i = 0; i < f->num_cascades; i++)
		c_clr(f->cascades[i]);

	c_clr(f->freecells);

	for (i = 0; i < NUM_SUITS * f->num_decks; i++)
		f->homecells[i] = NUM_CARDS;

	srand(time(NULL));
	deal(f);
}

int can_stack(char a, char b)
{
	return getrank(b) - getrank(a) == 1 &&
		   isblack(a) != isblack(b);
}

int cascade_to_cascade(int srci, int dsti)
{
	struct cascade_t *src = cascades[srci],
					 *dst = cascades[dsti];
	int valid = dst->size == 0 ||
				can_stack(c_peek(src), c_peek(dst));

	if (valid)
	{
		gotocc(srci, src->size - 1);
		carderase();
		c_push(dst, c_pop(src));
		gotocc(dsti, dst->size - 1);
		cardprint(c_peek(dst), 0);
	}

	return valid;
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

int freecell_to_cascade(int srci, int dsti)
{
	struct cascade_t *dst = cascades[dsti];
	int valid = dst->size == 0 ||
				can_stack(freecells->cards[srci], c_peek(dst));

	if (valid)
	{
		c_push(dst, c_rm(freecells, srci));
		gotocc(dsti, dst->size - 1);
		cardprint(c_peek(dst), 0);
		refresh_freecells();
	}

	return valid;
}

int cascade_to_freecell(int srci)
{
	struct cascade_t *src = cascades[srci];
	int valid = freecells->size < NUM_FREECELLS;

	if (valid)
	{
		c_push(freecells, c_pop(src));
		gotocc(srci, src->size);
		carderase();
		refresh_freecells();
	}

	return valid;
}

int to_homecell(int srci, enum selection_types selection)
{
	char a, b;
	int valid, i, dsti;

	a = selection == SELECT_FREECELL
			? freecells->cards[srci]
			: c_peek(cascades[srci]);

	for (i = 0; i < NUM_DECKS; i++)
	{
		dsti = getsuit(a) * NUM_DECKS + i;
		b = homecells[dsti];
		valid = (getrank(a) == 0 && b == NUM_CARDS) ||
				(getrank(a) - getrank(b) == 1);
		if (valid)
		{
			switch (selection)
			{
			case SELECT_FREECELL:
				homecells[dsti] = c_rm(freecells, srci);
				refresh_freecells();
				break;
			case SELECT_CASCADE:
				homecells[dsti] = c_pop(cascades[srci]);
				gotocc(srci, cascades[srci]->size);
				carderase();
				break;
			}
			refresh_homecells();
			return 1;
		}
	}

	return 0;
}
