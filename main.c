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
	struct game G;
	struct transfer T;
	size_t i;

	f_init(&G);
	g_init();
	T.srct = T.dstt = ST_NONE;

#if DEBUG
	f_newgame(&G, DOUBLE_DECK);
	G.num_freecells = 0;
	G.num_cascades = 10;
	for (i = 0; i < G.num_cascades; i++)
		c_clr(G.cascades[i]);

	for (i = 0; i < 13; i++)
		c_push(G.cascades[0], getcard(NUM_RANKS - 1 - i, i % 2 << 1));
	c_push(G.cascades[1], getcard(NUM_RANKS - 1, 0));
	refresh(&G);
#else
	f_newgame(&G, DOUBLE_DECK);
	refresh(&G);
#endif

	while (true)
	{
		accept_keypress(&G, &T);
		i = EXTRA ? auto_transfer(&G, &T) : f_transfer(&G, &T);

		if (i)
		{
			update_display(&G, &T);
			T.srct = T.dstt = ST_NONE;
		}
		else
		{
			T.dstt = ST_NONE;
			update_display(&G, &T);
		}
	}
}
