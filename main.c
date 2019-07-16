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

int confirm_yn(char *message)
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

void goto_freecell(int index)
{
	gotoxy(3, 3 + 2 * index);
}

void goto_homecell(int index)
{
	gotoxy(73, 3 + 2 * index);
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
	for (i = 0; i < F.num_cascades; i++)
	{
		gotocc(i, -2);
		cprintf("  %d", i);
	}
	/* print the cascades */
	for (i = 0; i < F.num_cascades; i++)
	{
		for (j = 0; j < F.cascades[i]->size; j++)
		{
			gotocc(i, j);
			cardprint(F.cascades[i]->cards[j], 0);
		}
	}
	refresh_freecells();
	refresh_homecells();
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

void init(void)
{
	char i;

	directvideo = 0;
	textmode(C80);

	for (i = 0; i < 10; i++)
		F.cascades[i] = cascade_new(MAX_CASCADE_SIZE);

	F.freecells = cascade_new(MAX_FREECELLS);

	for (i = 0; i < NUM_SUITS * 2; i++)
		F.homecells[i] = NUM_CARDS;
}

#define DEBUG 1
int main(void)
{
	enum selection_types selection = SELECT_NONE;
	size_t srci, i, key;

	init();
#if DEBUG
	for (i = 12; i >= 8; i--)
		c_push(F.cascades[0], getcard(i, i % 2 << 1));
	c_push(F.cascades[1], getcard(12, 0));
#else
	newgame(&F, DOUBLE_DECK);
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
				if (F.freecells->size)
				{
					selection = SELECT_FREECELL;
					srci = F.freecells->size - 1;
					goto_freecell(srci);
					cardprint(freecells->cards[srci], 1);
				}
				break;
			case SELECT_FREECELL:
				goto_freecell(srci);
				cardprint(freecells->cards[srci], 0);
				if (srci)
				{
					srci--;
					goto_freecell(srci);
					cardprint(freecells->cards[srci], 1);
				}
				else
					selection = SELECT_NONE;
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
					else if (cascade_to_cascade_m(srci, i, 80))
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
}
