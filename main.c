#include <stdio.h>
#include <stdlib.h>

#include "freecell.h"
#include "graphics.h"

#define DEBUG 0

int main(void)
{
	struct freecell_t F;
	struct transfer_t T;

	f_init(&F);
	g_init();
	T.srcsel = T.dstsel = S_NONE;

#if DEBUG
	for (i = 12; i >= 8; i--)
		c_push(F.cascades[0], getcard(i, i % 2 << 1));
	c_push(F.cascades[1], getcard(12, 0));
#else
	f_newgame(&F, DOUBLE_DECK);
	refresh(&F);
#endif

	while (1)
	{
		accept_keypress(&F, &T);

		if (f_transfer(&F, &T))
		{
			update_display(&F, &T);
			T.srcsel = T.dstsel = S_NONE;
		}
		else
		{
			T.dstsel = S_NONE;
			update_display(&F, &T);
		}
	}
}