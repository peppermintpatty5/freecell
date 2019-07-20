#include <stdio.h>
#include <stdlib.h>

#include "conio.h"
#include "card.h"
#include "cascade.h"
#include "freecell.h"
#include "main.h"

static const char BORDER_TOP[] = {201, 205, 205, 205, 205, 205, 205, 187, 0},
				  BORDER_MID[] = {199, 196, 196, 196, 196, 196, 196, 182, 0},
				  BORDER_BOT[] = {200, 205, 205, 205, 205, 205, 205, 188, 0},
				  BORDER_L_R[] = {186, 32, 32, 32, 32, 32, 32, 186, 0};

static struct freecell_t F;
static struct transfer_t T;

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
 * Send the cursor to a card within a cascade.
 */
void goto_cascade(int cascadei, int cardi)
{
	switch (F.num_decks)
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
static void goto_freecell(int index)
{
	switch (F.num_decks)
	{
	case 1:
		gotoxy(3, 4 * index + 4);
		break;
	case 2:
		gotoxy(3, 2 * index + 3);
		break;
	}
}

static void goto_homecell(int index)
{
	switch (F.num_decks)
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

void refresh(void)
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
	for (i = 0; i < F.num_cascades; i++)
	{
		goto_cascade(i, -2);
		cprintf("  %d", i);
	}
	/* print the cascades */
	for (i = 0; i < F.num_cascades; i++)
	{
		for (j = 0; j < F.cascades[i]->size; j++)
		{
			goto_cascade(i, j);
			cardprint(F.cascades[i]->cards[j], 0);
		}
	}
	refresh_freecells();
	refresh_homecells();
}

void refresh_cascade_tail(size_t index, int select)
{
	struct cascade_t *cascade = F.cascades[index];
	char a = c_peek(cascade);

	if (cascade->size)
	{
		goto_cascade(index, cascade->size - 1);
		cardprint(a, select);
	}
	if (cascade->size < MAX_CASCADE_SIZE)
	{
		goto_cascade(index, cascade->size);
		carderase();
	}
}

void refresh_freecells(void)
{
	size_t i;

	for (i = 0; i < F.num_freecells; i++)
	{
		goto_freecell(i);
		if (i < F.freecells->size)
			cardprint(F.freecells->cards[i], 0);
		else
			carderase();
	}
}

void refresh_homecells(void)
{
	char i;

	for (i = 0; i < NUM_SUITS * F.num_decks; i++)
	{
		goto_homecell(i);
		if (F.homecells[i] == NUM_CARDS)
		{
			textcolor(DARKGRAY);
			textbackground(BLACK);
			cprintf(" %c", i / F.num_decks + '\3');
		}
		else
			cardprint(F.homecells[i], 0);
	}
}

/**
 * Only re-print the areas of the screen which are necessary.
 */
static void quick_refresh()
{
}

void init(void)
{
	directvideo = 0;
	textmode(C80);

	f_init(&F);
}

#define DEBUG 0
int main(void)
{
	int key;

	init();

#if DEBUG
	for (i = 12; i >= 8; i--)
		c_push(F.cascades[0], getcard(i, i % 2 << 1));
	c_push(F.cascades[1], getcard(12, 0));
#else
	T.srci = T.dsti = S_NONE;
	f_newgame(&F, DOUBLE_DECK);
#endif

	refresh();
	do
	{
		hidecursor;
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
				T.srci = T.dsti = S_NONE;
				f_newgame(&F, SINGLE_DECK);
				refresh();
			}
			break;
		case 'a':
		case 'A':
			switch (T.srcsel)
			{
			case S_NONE:
				/* cannot select empty freecells */
				if (F.freecells->size)
				{
					T.srci = F.freecells->size - 1;
					goto_freecell(T.srci);
					cardprint(F.freecells->cards[T.srci], 1);
					T.srcsel = S_FREECELL;
				}
				break;
			case S_FREECELL:
				goto_freecell(T.srci);
				cardprint(F.freecells->cards[T.srci], 0);
				if (T.srci)
				{
					T.srci--;
					goto_freecell(T.srci);
					cardprint(F.freecells->cards[T.srci], 1);
				}
				else
					T.srcsel = S_NONE;
				break;
			case S_CASCADE:
				if (cascade_to_freecell(&F, T.srci))
				{
					refresh_cascade_tail(T.srci, 0);
					refresh_freecells();
					T.srcsel = S_NONE;
				}
				break;
			}
			break;
		case 'b':
		case 'B':
			switch (T.srcsel)
			{
			case S_NONE:
				/* cannot select the homecells */
				break;
			case S_FREECELL:
				if (to_homecell(&F, T.srci, S_FREECELL))
					T.srcsel = S_NONE;
				break;
			case S_CASCADE:
				if (to_homecell(&F, T.srci, S_CASCADE))
					T.srcsel = S_NONE;
				break;
			}
			break;
		default:
			key -= '0';
			if (key >= 0 && key < F.num_cascades)
			{
				switch (T.srcsel)
				{
				case S_NONE:
					/* cannot select empty cascade */
					if (F.cascades[T.srci = key]->size)
					{
						refresh_cascade_tail(T.srci, 1);
						T.srcsel = S_CASCADE;
					}
					break;
				case S_FREECELL:
					if (freecell_to_cascade(&F, T.srci, key))
					{
						refresh_freecells();
						refresh_cascade_tail(key, 0);
						T.srcsel = S_NONE;
					}
					break;
				case S_CASCADE:
					if (key == T.srci)
					{
						refresh_cascade_tail(T.srci, 0);
						T.srcsel = S_NONE;
					}
					else if (cascade_to_cascade(&F, T.srci, key))
					{
						refresh_cascade_tail(T.srci, 0);
						refresh_cascade_tail(key, 0);
						T.srcsel = S_NONE;
					}
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
	textmode(LASTMODE);
	return EXIT_SUCCESS;
}
