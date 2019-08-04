#include <conio.h>

#include "card.h"

void cardprint(Card card, char select)
{
	char str_rank[3];

	str_rank[0] = "A234567891JQK"[getrank(card)]; /* Hardcoded values */
	str_rank[1] = str_rank[0] == '1' ? '0' : ' '; /* The '0' in '10'  */
	str_rank[2] = '\0';							  /* Null terminator  */

	textcolor(isblack(card) ? CYAN : LIGHTRED);
	textbackground(BLACK);
	cprintf(" %c%c%s", getsuit(card) + '\3', select ? ':' : ' ', str_rank);
}

void carderase(void)
{
	textcolor(BLACK);
	textbackground(BLACK);
	cprintf("     ");
}
