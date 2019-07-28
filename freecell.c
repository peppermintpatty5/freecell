#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "conio.h"
#include "dos.h"
#include "freecell.h"

void f_init(struct freecell_t *f)
{
	size_t i;

	for (i = 0; i < 10; i++)
		f->cascades[i] = cascade_new(MAX_CASCADE_SIZE);

	f->freecells = cascade_new(MAX_FREECELLS);
}

/**
 * Clears the cascades and deals 'f->num_decks' shuffled decks.
 */
static void deal(struct freecell_t *f)
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
		/* remember where the last card was dealt */
		j_ = (j + j_) % f->num_cascades;
	}
}

void f_newgame(struct freecell_t *f, enum game_types gt)
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

int f_transfer(struct freecell_t *f, struct transfer_t *t)
{
	size_t i;
	char a, *b;
	int valid;
	struct cascade_t *dst;

	switch (t->srcsel)
	{
	case S_CASCADE:
		a = c_peek(f->cascades[t->srci]);
		break;
	case S_FREECELL:
		a = f->freecells->cards[t->srci];
		break;
	default:
		return 0;
	}

	switch (t->dstsel)
	{
	case S_CASCADE:
		dst = f->cascades[t->dsti];
		if (valid = !dst->size || can_stack(a, c_peek(dst)))
			c_push(dst, a);
		break;
	case S_FREECELL:
		dst = f->freecells;
		if (valid = dst->size < f->num_freecells)
			c_push(dst, a);
		break;
	case S_HOMECELL:
		for (i = 0; i < f->num_decks; i++)
		{
			b = &f->homecells[getsuit(a) * f->num_decks + i];
			valid = (getrank(a) == 0 && *b == NUM_CARDS) ||
					(getrank(a) - getrank(*b) == 1);
			if (valid)
			{
				*b = a; /* hehehe */
				break;
			}
		}
		break;
	default:
		return 0;
	}

	if (valid)
	{
		switch (t->srcsel)
		{
		case S_CASCADE:
			c_pop(f->cascades[t->srci]);
			break;
		case S_FREECELL:
			c_rm(f->freecells, t->srci);
			break;
		}
	}

	return valid;
}

int can_stack(char a, char b)
{
	return getrank(b) - getrank(a) == 1 && isblack(a) != isblack(b);
}
