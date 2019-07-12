#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "conio.h"
#include "card.h"
#include "cascade.h"
#include "freecell.h"

/**
 * Helper function that returns the number of continuously stackable cards from
 * the top of the cascade. The minimum value of 1 indicates that the top card
 * is not stackable on the card beneath it. By contrast, a value equal to
 * 'src->size' indicates that the entire cascade is a valid FreeCell stack.
 */
static size_t count_stack_streak(struct cascade_t *src);

static const char BORDER_TOP[] = {201, 205, 205, 205, 205, 205, 205, 187, 0},
				  BORDER_MID[] = {199, 196, 196, 196, 196, 196, 196, 182, 0},
				  BORDER_BOT[] = {200, 205, 205, 205, 205, 205, 205, 188, 0},
				  BORDER_L_R[] = {186, 32, 32, 32, 32, 32, 32, 186, 0};

static struct cascade_t *cascades[NUM_CASCADES];
static struct cascade_t *freecells;
static char homecells[NUM_HOMECELLS];

void hidecursor(void)
{
	gotoxy(1, 1);
}

int confirm_yn(char *message)
{
	signed int status = -1;
	gotoxy(1, 25);
	textcolor(LIGHTGREEN);
	textbackground(BLACK);
	cprintf("%s (Y/N)", message);
	hidecursor();

	while (status < 0)
	{
		switch (getch())
		{
		case 'y':
		case 'Y':
			status = 1;
			break;
		case 'n':
		case 'N':
			status = 0;
			break;
		default:
			break;
		}
	}
	gotoxy(1, 25);
	delline();

	return status;
}

void goto_freecell(int index)
{
	gotoxy(3, 3 + 2 * index);
}

void gotocc(char cascadei, char cardi)
{
	gotoxy(cascadei * 6 + 11, cardi + 3);
}

void pretty_borders(char x, char y)
{
	char i;

	textcolor(BLUE);
	textbackground(BLACK);

	for (i = 0; i < 17; i++)
	{
		gotoxy(x, i + y);
		if (i == 0)
			cprintf(BORDER_TOP);
		else if (i == 16)
			cprintf(BORDER_BOT);
		else if (i % 4 == 0)
			cprintf(BORDER_MID);
		else
			cprintf(BORDER_L_R);
	}
}

/**
 * TODO split up into smaller functions
 */
void refresh(void)
{
	char i, j;
	/* clear the screen */
	textcolor(BLACK);
	textbackground(BLACK);
	clrscr();
	pretty_borders(2, 2);
	pretty_borders(72, 2);
	/* numbers on the top of the screen */
	textcolor(LIGHTGRAY);
	gotoxy(5, 1);
	cprintf("A");
	gotoxy(75, 1);
	cprintf("B");
	for (i = 0; i < NUM_CASCADES; i++)
	{
		gotocc(i, -2);
		cprintf("  %d", i);
	}
	/* print the cascades */
	for (i = 0; i < NUM_CASCADES; i++)
	{
		for (j = 0; j < cascades[i]->size; j++)
		{
			gotocc(i, j);
			cardprint(cascades[i]->cards[j], 0);
		}
	}
	refresh_freecells();
	refresh_homecells();
}

void refresh_freecells(void)
{
	char i;

	for (i = 0; i < NUM_FREECELLS; i++)
	{
		goto_freecell(i);
		if (i < freecells->size)
			cardprint(freecells->cards[i], 0);
		else
			carderase();
	}
}

void refresh_homecells(void)
{
	char i;

	for (i = 0; i < NUM_HOMECELLS; i++)
	{
		gotoxy(73, 3 + 2 * i);
		if (homecells[i] == NUM_CARDS)
		{
			textcolor(DARKGRAY);
			textbackground(BLACK);
			cprintf(" %c", i / NUM_DECKS + '\3');
		}
		else
			cardprint(homecells[i], 0);
	}
}

void deal(unsigned int n)
{
	unsigned int i, j, t, i_ = 0;
	char deck[NUM_CARDS];

	for (i = 0; i < NUM_CARDS; i++)
		deck[51 - i] = i;
	while (n-- > 0)
	{
		/* shuffle */
		for (i = NUM_CARDS - 1; i > 0; i--)
		{
			j = rand() % i;
			t = deck[i];
			deck[i] = deck[j];
			deck[j] = t;
		}
		/* deal */
		for (i = 0; i < NUM_CARDS; i++)
			c_push(cascades[(i + i_) % NUM_CASCADES], deck[i]);
		i_ = i % NUM_CASCADES;
	}
}

void newgame(void)
{
	char i;
	/* clear and deal cascades */
	for (i = 0; i < NUM_CASCADES; i++)
		c_clr(cascades[i]);
	srand(time(NULL));
	deal(NUM_DECKS);
	/* clear freecells */
	c_clr(freecells);
	/* clear homecells */
	for (i = 0; i < NUM_HOMECELLS; i++)
		homecells[i] = NUM_CARDS;
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

int cascade_to_cascade_m(int srci, int dsti)
{
	struct cascade_t *src = cascades[srci],
					 *dst = cascades[dsti];
	size_t count = count_stack_streak(src),
		   nfree = NUM_FREECELLS - freecells->size;

	if (dst->size)
	{
		while (count && !can_stack(src->cards[src->size - count], c_peek(dst)))
			count--;
	}
	else
	{
		if (nfree < count)
			count = nfree;
	}
}

static size_t count_stack_streak(struct cascade_t *src)
{
	size_t i;

	for (i = src->size - 1; i > 0; i--)
	{
		if (!can_stack(src->cards[i], src->cards[i - 1]))
			break;
	}

	return src->size - i;
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

void init(void)
{
	char i;

	directvideo = 0;
	textmode(C80);

	for (i = 0; i < NUM_CASCADES; i++)
		cascades[i] = cascade_new(MAX_CASCADE_SIZE);
	freecells = cascade_new(NUM_FREECELLS);
}

#define DEBUG 1
int main(void)
{
#if DEBUG
	struct cascade_t *s = cascade_new(10);
	size_t i;

	c_push(s, getcard(10, 2));
	c_push(s, getcard(9, 0));
	c_push(s, getcard(6, 3));
	c_push(s, getcard(8, 2));
	c_push(s, getcard(7, 0));

	for (i = 0; i < s->size; i++)
		cardprint(s->cards[i], 0);
	printf("Stack streak: %d\n", count_stack_streak(s));
#else
	enum selection_types selection = SELECT_NONE;
	signed int srci, i, key;

	init();
	newgame();
	refresh();

	do
	{
		hidecursor();
		key = getch();
		switch (key)
		{
		case 'q':
		case 'Q':
			if (confirm_yn("Quit game?"))
				goto RET;
			break;
		case 'n':
		case 'N':
			if (confirm_yn("Start a new game?"))
			{
				selection = SELECT_NONE;
				newgame();
				refresh();
			}
			break;
		case 'a':
		case 'A':
			switch (selection)
			{
			case SELECT_NONE:
				/* cannot select empty freecells */
				if (freecells->size)
				{
					selection = SELECT_FREECELL;
					srci = freecells->size - 1;
					goto_freecell(srci);
					cardprint(freecells->cards[srci], 1);
				}
				break;
			case SELECT_FREECELL:
				goto_freecell(srci);
				cardprint(freecells->cards[srci], 0);
				srci--;
				if (srci < 0)
					selection = SELECT_NONE;
				else
				{
					goto_freecell(srci);
					cardprint(freecells->cards[srci], 1);
				}
				break;
			case SELECT_CASCADE:
				if (cascade_to_freecell(srci))
					selection = SELECT_NONE;
				break;
			}
			break;
		case 'b':
		case 'B':
			switch (selection)
			{
			case SELECT_NONE:
				/* cannot select the homecells */
				break;
			case SELECT_FREECELL:
				if (to_homecell(srci, SELECT_FREECELL))
					selection = SELECT_NONE;
				break;
			case SELECT_CASCADE:
				if (to_homecell(srci, SELECT_CASCADE))
					selection = SELECT_NONE;
				break;
			}
			break;
		default:
			i = key - '0';
			if (i >= 0 && i < NUM_CASCADES)
			{
				switch (selection)
				{
				case SELECT_NONE:
					/* cannot select empty cascade */
					if (cascades[i]->size)
					{
						selection = SELECT_CASCADE;
						srci = i;
						gotocc(srci, cascades[srci]->size - 1);
						cardprint(c_peek(cascades[srci]), 1);
					}
					break;
				case SELECT_FREECELL:
					if (freecell_to_cascade(srci, i))
						selection = SELECT_NONE;
					break;
				case SELECT_CASCADE:
					if (i == srci)
					{
						gotocc(srci, cascades[srci]->size - 1);
						cardprint(c_peek(cascades[srci]), 0);
						selection = SELECT_NONE;
					}
					else if (cascade_to_cascade(srci, i))
						selection = SELECT_NONE;
					break;
				}
			}
			break;
		}
	} while (1);

RET:
	textbackground(BLACK);
	textcolor(LIGHTGRAY);
	clrscr();
	return EXIT_SUCCESS;
#endif
}
