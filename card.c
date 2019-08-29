#include <conio.h>

#include "card.h"

void cardprint(Card card, bool select)
{
	char str[6];

	str[0] = ' ';
	str[1] = '\x03' + getsuit(card);
	str[2] = select ? ':' : ' ';
	str[3] = "A234567891JQK"[getrank(card)];
	str[4] = str[3] == '1' ? '0' : ' ';
	str[5] = '\x00';

	textcolor(isblack(card) ? CYAN : LIGHTRED);
	textbackground(BLACK);
	cprintf(str);
}

void carderase(void)
{
	textcolor(BLACK);
	textbackground(BLACK);
	cprintf("     ");
}
