#ifndef _FREECELL_H_
#define _FREECELL_H_

#define MAX_CASCADE_SIZE (24)
#define MAX_CASCADES (10)
#define MAX_FREECELLS (8)

enum selection_types
{
	SELECT_NONE,
	SELECT_FREECELL,
	SELECT_CASCADE
};

enum game_types
{
	SINGLE_DECK,
	DOUBLE_DECK
};

/**
 * Struct containing the data for a game of FreeCell. Supports both single and
 * double deck FreeCell variations.
 */
struct freecell_t
{
	size_t num_decks;
	size_t num_cascades;
	size_t num_freecells;
	struct cascade_t *cascades[MAX_CASCADES];
	struct cascade_t *freecells;
	char homecells[NUM_SUITS * 2];
};

/**
 * Deal n many shuffled decks to the cascades.
 */
void deal(struct freecell_t *f);

void newgame(struct freecell_t *f, enum game_types gt);

/**
 * Determines if card a can be stacked on top of card b.
 */
int can_stack(char a, char b);

/**
 * Attempt to transfer card from one cascade to another, according to the rules
 * of FreeCell. Returns 0 if failed, non-zero if succeeded.
 * 
 * Destination index may refer to empty cascade, while source index may not.
 * 
 * Also performs graphical updates (hopefully).
 */
int cascade_to_cascade(int srci, int dsti);

/**
 * A (probably) recursive function that moves multiple cards from one stack to
 * another.
 * 
 * Destination index may refer to empty cascade, while source index may not.
 * 
 * Also does cool animation if you want to see it.
 */
int cascade_to_cascade_m(int srci, int dsti, int delay_ms);

int freecell_to_cascade(int srci, int dsti);

int cascade_to_freecell(int srci);

/**
 * Attemps to move card from a freecell or cascade to the homecells.
 */
int to_homecell(int srci, enum selection_types selection);

#endif
