#include <stdio.h>
#include <stdlib.h>

#include <conio.h>

#include "card.h"
#include "cascade.h"
#include "graphics.h"

#define hidecursor gotoxy(1, 1)

static const char BORDER_TOP[] = {201, 205, 205, 205, 205, 205, 205, 187, 0},
				  BORDER_MID[] = {199, 196, 196, 196, 196, 196, 196, 182, 0},
				  BORDER_BOT[] = {200, 205, 205, 205, 205, 205, 205, 188, 0},
				  BORDER_L_R[] = {186, 32, 32, 32, 32, 32, 32, 186, 0};

/**
 * Send the cursor to a card within a cascade.
 */
static void goto_cascade(FreeCell *f, int cascadei, int cardi)
{
	switch (f->num_decks)
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
static void goto_freecell(FreeCell *f, int index)
{
	switch ((f->num_freecells + 3) / 4)
	{
	case 1: /* [1, 4] */
		gotoxy(3, 4 * index + 4);
		break;
	case 2: /* [5, 8] */
		gotoxy(3, 2 * index + 3);
		break;
	}
}

static void goto_homecell(FreeCell *f, int index)
{
	switch (f->num_decks)
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

int confirm_yn(const char *message)
{
	int status = 2;

	gotoxy(1, 25);
	textcolor(LIGHTGREEN);
	textbackground(BLACK);
	cprintf("%s (Y/N)", message);
	hidecursor;

	while (status == 2)
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

/**
 * Updates only the top of the selected cascade on the display.
 */
static void update_cascade(FreeCell *f, size_t index, int select)
{
	Cascade *cascade = f->cascades[index];
	char a = c_peek(cascade);

	if (cascade->size)
	{
		goto_cascade(f, index, cascade->size - 1);
		cardprint(a, select);
	}
	if (cascade->size < MAX_CASCADE_SIZE)
	{
		goto_cascade(f, index, cascade->size);
		carderase();
	}
}

static void refresh_freecells(FreeCell *f, size_t index, int select)
{
	size_t i;

	for (i = 0; i < f->num_freecells; i++)
	{
		goto_freecell(f, i);
		if (i < f->freecells->size)
			cardprint(f->freecells->cards[i], select && i == index);
		else
			carderase();
	}
}

static void refresh_homecells(FreeCell *f)
{
	char i;

	for (i = 0; i < NUM_SUITS * f->num_decks; i++)
	{
		goto_homecell(f, i);
		if (f->homecells[i] == NUM_CARDS)
		{
			textcolor(DARKGRAY);
			textbackground(BLACK);
			cprintf(" %c", i / f->num_decks + '\3');
		}
		else
			cardprint(f->homecells[i], 0);
	}
}

void update_display(FreeCell *f, const Transfer *t)
{
	switch (t->srct)
	{
	case ST_CASCADE:
		update_cascade(f, t->srci, t->dstt == ST_NONE);
		break;
	case ST_FREECELL:
		refresh_freecells(f, t->srci, t->dstt == ST_NONE);
		break;
	}

	switch (t->dstt)
	{
	case ST_CASCADE:
		update_cascade(f, t->dsti, 0);
		break;
	case ST_FREECELL:
		refresh_freecells(f, t->srci, 0);
		break;
	case ST_HOMECELL:
		refresh_homecells(f);
		break;
	}
}

void refresh(FreeCell *f)
{
	size_t i, j;
	int select;

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
	for (i = 0; i < f->num_cascades; i++)
	{
		goto_cascade(f, i, -2);
		cprintf("  %d", i);
	}

	/* print the cascades */
	for (i = 0; i < f->num_cascades; i++)
	{
		for (j = 0; j < f->cascades[i]->size; j++)
		{
			goto_cascade(f, i, j);
			cardprint(f->cascades[i]->cards[j], 0);
		}
	}
	refresh_freecells(f, 0, 0);
	refresh_homecells(f);
}

void accept_keypress(FreeCell *f, Transfer *t)
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
			f_newgame(f, SINGLE_DECK);
			refresh(f);
		}
		break;
	case 'a':
	case 'A':
		switch (t->srct)
		{
		case ST_NONE:
			/* cannot select empty freecells */
			if (f->freecells->size)
			{
				t->srci = f->freecells->size - 1;
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
		if (key >= 0 && key < f->num_cascades)
		{
			switch (t->srct)
			{
			case ST_NONE:
				/* cannot select empty cascade */
				if (f->cascades[t->srci = key]->size)
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
