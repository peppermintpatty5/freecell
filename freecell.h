#ifndef _FREECELL_H_
#define _FREECELL_H_

#include "card.h"
#include "cascade.h"

/**
 * A limitation of the 80x25 textmode, which is also the highest possible stack
 * in a double deck FreeCell game.
 */
#define MAX_CASCADE_SIZE (23)
#define MAX_DECKS (2)
#define MAX_CASCADES (10)
#define MAX_FREECELLS (8)

enum selection_types
{
	S_NONE,
	S_CASCADE,
	S_FREECELL,
	S_HOMECELL,
};

enum game_types
{
	SINGLE_DECK,
	DOUBLE_DECK
};

/**
 * Contains the data for a game of FreeCell. Supports both single and double
 * deck FreeCell variations.
 */
struct freecell_t
{
	size_t num_decks;
	size_t num_cascades;
	size_t num_freecells;
	struct cascade_t *cascades[MAX_CASCADES];
	struct cascade_t *freecells;
	char homecells[NUM_SUITS * MAX_DECKS];
};

/**
 * Contains the data for a card transfer from 
 */
struct transfer_t
{
	size_t srci;
	size_t dsti;
	enum selection_types srcsel;
	enum selection_types dstsel;
};

/**
 * Clears the cascades and deals 'f->num_decks' shuffled decks.
 */
void deal(struct freecell_t *f);

void newgame(struct freecell_t *f, enum game_types gt);

int f_transfer(struct freecell_t *f, struct transfer_t *t);

/**
 * Determines if card 'a' can be stacked on top of card 'b'.
 */
int can_stack(char a, char b);

int cascade_to_cascade(struct freecell_t *f, size_t srci, size_t dsti);

int cascade_to_freecell(struct freecell_t *f, size_t srci);

int cascade_to_homecell(struct freecell_t *f, size_t srci);

int freecell_to_cascade(struct freecell_t *f, size_t srci, size_t dsti);

/**
 * Attemps to move card from a freecell or cascade to the homecells.
 */
int to_homecell(struct freecell_t *f, int srci, enum Sion_types sel);

#endif
