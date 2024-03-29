#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <conio.h>

#include "freecell.h"

void f_init(struct game *f)
{
	size_t i;

	for (i = 0; i < MAX_CASCADES; i++)
		f->cascades[i] = cascade_new(MAX_CASCADE_SIZE);

	f->freecells = cascade_new(MAX_FREECELLS);
}

/**
 * Clears the cascades and deals 'f->num_decks' shuffled decks.
 */
static void deal(struct game *f)
{
	size_t i, j, r, j_ = 0;
	Card deck[NUM_CARDS], c;

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
		/* remember where the last card was dealt */
		j_ = (j + j_) % f->num_cascades;
	}
}

void f_newgame(struct game *f, enum gametype gt)
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

bool f_transfer(struct game *f, const struct transfer *t)
{
	size_t i;
	Card a, *b;
	struct cascade *dst;
	bool valid;

	if (t->srct == t->dstt && t->srci == t->dsti)
		return true;

	switch (t->srct)
	{
	case ST_CASCADE:
		a = c_peek(f->cascades[t->srci]);
		break;
	case ST_FREECELL:
		a = f->freecells->cards[t->srci];
		break;
	default:
		return 0;
	}

	switch (t->dstt)
	{
	case ST_CASCADE:
		dst = f->cascades[t->dsti];
		valid = !dst->size || can_stack(a, c_peek(dst));
		if (valid)
			c_push(dst, a);
		break;
	case ST_FREECELL:
		dst = f->freecells;
		valid = dst->size < f->num_freecells;
		if (valid)
			c_push(dst, a);
		break;
	case ST_HOMECELL:
		for (i = 0; i < f->num_decks; i++)
		{
			b = &f->homecells[getsuit(a) * f->num_decks + i];
			valid = (getrank(a) == 0 && *b == NUM_CARDS) ||
					(getrank(a) - getrank(*b) == 1);
			if (valid)
			{
				*b = a;
				break;
			}
		}
		break;
	default:
		return false;
	}

	if (valid)
	{
		switch (t->srct)
		{
		case ST_CASCADE:
			c_pop(f->cascades[t->srci]);
			break;
		case ST_FREECELL:
			c_rm(f->freecells, t->srci);
			break;
		}
	}

	return valid;
}
