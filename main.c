#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "extra.h"
#include "freecell.h"
#include "graphics.h"

#define DEBUG (1)
#define EXTRA (1)

int main(void)
{
	FreeCell F;
	Transfer T;
	size_t i;

	f_init(&F);
	g_init();
	T.srct = T.dstt = ST_NONE;

#if DEBUG
	f_newgame(&F, DOUBLE_DECK);
	F.num_freecells = 0;
	F.num_cascades = 10;
	for (i = 0; i < F.num_cascades; i++)
		c_clr(F.cascades[i]);

	for (i = 0; i < 13; i++)
		c_push(F.cascades[0], getcard(NUM_RANKS - 1 - i, i % 2 << 1));
	c_push(F.cascades[1], getcard(NUM_RANKS - 1, 0));
	refresh(&F);
#else
	f_newgame(&F, DOUBLE_DECK);
	refresh(&F);
#endif

	while (true)
	{
		accept_keypress(&F, &T);
		i = EXTRA ? auto_transfer(&F, &T) : f_transfer(&F, &T);

		if (i)
		{
			update_display(&F, &T);
			T.srct = T.dstt = ST_NONE;
		}
		else
		{
			T.dstt = ST_NONE;
			update_display(&F, &T);
		}
	}
}
