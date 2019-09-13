#include <stdio.h>
#include <stdlib.h>

#include <conio.h>

#include "card.h"
#include "cascade.h"
#include "graphics.h"

#define hidecursor gotoxy(1, 1)

#define BORDER_TOP "\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xBB"
#define BORDER_MID "\xC7\xC4\xC4\xC4\xC4\xC4\xC4\xB6"
#define BORDER_BOT "\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xBC"
#define BORDER_L_R "\xBA\x20\x20\x20\x20\x20\x20\xBA"

/**
 * Send the cursor to a card within a cascade.
 */
static void goto_cascade(const struct game *g, int cascadei, int cardi)
{
	switch (g->num_decks)
	{
	case 1:
		gotoxy(6 * cascadei + 17, cardi + 3);
		break;
	case 2:
		gotoxy(6 * cascadei + 11, cardi + 3);
		break;
	}
}

/**
 * Move the cursor to the nth freecell.
 */
static void goto_freecell(const struct game *g, int index)
{
	int y;
	const int case3[] = {3, 4, 5, 7, 8, 9, 11, 12, 13, 15, 16, 17};

	switch ((g->num_freecells + 3) / 4)
	{
	case 1: /* [1, 4] */
		y = 4 * index + 4;
		break;
	case 2: /* [5, 8] */
		y = 2 * index + 3;
		break;
	case 3: /* [9, 12] */
		y = case3[index];
		break;
	}

	gotoxy(3, y);
}

static void goto_homecell(const struct game *g, int index)
{
	switch (g->num_decks)
	{
	case 1:
		gotoxy(73, 4 * index + 4);
		break;
	case 2:
		gotoxy(73, 2 * index + 3);
		break;
	}
}

static void pretty_borders(int x, int y)
{
	int i;

	textcolor(BLUE);
	textbackground(BLACK);

	for (i = 0; i <= 16; i++)
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

bool confirm_yn(const char *message)
{
	int buf[80];
	bool done = false, status;

	gettext(1, 25, 80, 25, buf);

	gotoxy(1, 25);
	textcolor(LIGHTBLUE);
	textbackground(BLACK);
	cprintf("%s (Y/N)", message);
	hidecursor;

	while (!done)
	{
		switch (getch())
		{
		case 'y':
		case 'Y':
			status = true;
			done = true;
			break;
		case 'n':
		case 'N':
			status = false;
			done = true;
			break;
		default:
			break;
		}
	}

	puttext(1, 25, 80, 25, buf);

	return status;
}

/**
 * Updates only the top of the selected cascade on the display.
 */
static void update_cascade(const struct game *g, size_t index, bool select)
{
	struct cascade *cascade = g->cascades[index];
	Card a = c_peek(cascade);

	if (cascade->size)
	{
		goto_cascade(g, index, cascade->size - 1);
		cardprint(a, select);
	}
	if (cascade->size < MAX_CASCADE_SIZE)
	{
		goto_cascade(g, index, cascade->size);
		carderase();
	}
}

static void refresh_freecells(const struct game *g, size_t index, bool select)
{
	size_t i;

	for (i = 0; i < g->num_freecells; i++)
	{
		goto_freecell(g, i);
		if (i < g->freecells->size)
			cardprint(g->freecells->cards[i], select && i == index);
		else
			carderase();
	}
}

static void refresh_homecells(const struct game *g)
{
	size_t i;

	for (i = 0; i < NUM_SUITS * g->num_decks; i++)
	{
		goto_homecell(g, i);
		if (g->homecells[i] == NUM_CARDS)
		{
			textcolor(DARKGRAY);
			textbackground(BLACK);
			cprintf(" %c", i / g->num_decks + '\x03');
		}
		else
			cardprint(g->homecells[i], false);
	}
}

void update_display(const struct game *g, const struct transfer *t)
{
	switch (t->srct)
	{
	case ST_CASCADE:
		update_cascade(g, t->srci, t->dstt == ST_NONE);
		break;
	case ST_FREECELL:
		refresh_freecells(g, t->srci, t->dstt == ST_NONE);
		break;
	}

	switch (t->dstt)
	{
	case ST_CASCADE:
		update_cascade(g, t->dsti, false);
		break;
	case ST_FREECELL:
		refresh_freecells(g, t->srci, false);
		break;
	case ST_HOMECELL:
		refresh_homecells(g);
		break;
	}
}

void refresh(const struct game *g)
{
	size_t i, j;

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
	for (i = 0; i < g->num_cascades; i++)
	{
		goto_cascade(g, i, -2);
		cprintf("  %d", i);
	}

	/* print the cascades */
	for (i = 0; i < g->num_cascades; i++)
	{
		for (j = 0; j < g->cascades[i]->size; j++)
		{
			goto_cascade(g, i, j);
			cardprint(g->cascades[i]->cards[j], false);
		}
	}
	refresh_freecells(g, 0, false);
	refresh_homecells(g);
}

void accept_keypress(const struct game *g, struct transfer *t)
{
	int key;

	hidecursor;
	switch (key = getch())
	{
	case 'q':
	case 'Q':
		if (confirm_yn("Quit game?"))
		{
			textbackground(BLACK);
			textcolor(LIGHTGRAY);
			clrscr();
			textmode(LASTMODE);
			exit(EXIT_SUCCESS);
		}
		break;
	case 'n':
	case 'N':
		if (confirm_yn("Start a new game?"))
		{
			t->srct = t->dstt = ST_NONE;
			f_newgame(g, SINGLE_DECK);
			refresh(g);
		}
		break;
	case 'a':
	case 'A':
		switch (t->srct)
		{
		case ST_NONE:
			/* cannot select empty freecells */
			if (g->freecells->size)
			{
				t->srci = g->freecells->size - 1;
				t->srct = ST_FREECELL;
			}
			break;
		case ST_FREECELL:
			if (t->srci)
				t->srci--;
			else
			{
				t->dsti = t->srci;
				t->dstt = ST_FREECELL;
			}
			break;
		default:
			t->dstt = ST_FREECELL;
			break;
		}
		break;
	case 'b':
	case 'B':
		switch (t->srct)
		{
		case ST_NONE:
			/* cannot select the homecells */
			break;
		default:
			t->dstt = ST_HOMECELL;
			break;
		}
		break;
	default:
		key -= '0';
		if (key >= 0 && key < g->num_cascades)
		{
			switch (t->srct)
			{
			case ST_NONE:
				/* cannot select empty cascade */
				if (g->cascades[t->srci = key]->size)
					t->srct = ST_CASCADE;
				break;
			default:
				t->dsti = key;
				t->dstt = ST_CASCADE;
				break;
			}
		}
		break;
	}
}

void g_init(void)
{
	directvideo = 0;
	textmode(C80);
}
