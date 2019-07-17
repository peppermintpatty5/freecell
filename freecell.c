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
		/* remember where the last card was dealt */
		j_ = (j + j_) % f->num_cascades;
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

int cascade_to_cascade(struct freecell_t *f, size_t srci, size_t dsti)
{
	struct cascade_t *src = f->cascades[srci],
					 *dst = f->cascades[dsti];
	int valid = !dst->size || can_stack(c_peek(src), c_peek(dst));

	if (valid)
		c_push(dst, c_pop(src));

	return valid;
}

int cascade_to_freecell(struct freecell_t *f, size_t srci)
{
	struct cascade_t *src = f->cascades[srci],
					 *dst = f->freecells;
	int valid = dst->size < f->num_freecells;

	if (valid)
		c_push(dst, c_pop(src));

	return valid;
}

int freecell_to_cascade(struct freecell_t *f, size_t srci, size_t dsti)
{
	struct cascade_t *src = f->freecells,
					 *dst = f->cascades[dsti];
	int valid = !dst->size || can_stack(src->cards[srci], c_peek(dst));

	if (valid)
		c_push(dst, c_rm(src, srci));

	return valid;
}

int to_homecell(struct freecell_t *f, int srci, enum selection_types sel)
{
	char a, b;
	int valid;
	size_t i, dsti;

	a = sel == SELECT_FREECELL
			? f->freecells->cards[srci]
			: c_peek(f->cascades[srci]);

	for (i = 0; i < f->num_decks; i++)
	{
		dsti = getsuit(a) * f->num_decks + i;
		b = f->homecells[dsti];
		valid = (getrank(a) == 0 && b == NUM_CARDS) ||
				(getrank(a) - getrank(b) == 1);
		if (valid)
		{
			switch (sel)
			{
			case SELECT_FREECELL:
				f->homecells[dsti] = c_rm(f->freecells, srci);
				refresh_freecells();
				break;
			case SELECT_CASCADE:
				f->homecells[dsti] = c_pop(f->cascades[srci]);
				gotocc(srci, f->cascades[srci]->size);
				carderase();
				break;
			}
			refresh_homecells();
			break;
		}
	}

	return valid;
}
