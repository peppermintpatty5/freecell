#include <stdio.h>
#include <stdlib.h>

#include <conio.h>

#include "card.h"

/**
 * For your reference!
 */
#define HEART '\3'
#define DIAMOND '\4'
#define CLUB '\5'
#define SPADE '\6'

void cardprint(char card, char select)
{
	char str_rank[3];

	str_rank[0] = "A234567891JQK"[getrank(card)]; /* Hardcoded values */
	str_rank[1] = getrank(card) == 9 ? '0' : ' '; /* The '0' in '10'  */
	str_rank[2] = '\0';							  /* Null terminator  */

	textcolor(isblack(card) ? CYAN : LIGHTRED);
	textbackground(BLACK);
	cprintf(" %c%c%s", getsuit(card) + 3, select ? ':' : ' ', str_rank);
}

void carderase(void)
{
	textcolor(BLACK);
	textbackground(BLACK);
	cprintf("     ");
}
