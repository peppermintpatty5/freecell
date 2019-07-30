#ifndef _FREECELL_H_
#define _FREECELL_H_

#include <stddef.h>

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

enum game_types
{
	SINGLE_DECK,
	DOUBLE_DECK
};

enum selection_types
{
	S_NONE,
	S_CASCADE,
	S_FREECELL,
	S_HOMECELL
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
 * Contains the data for transferring a single card. 'scri' and 'dsti' are the
 * source/destitnation indices of the locations specified by 'srcsel' and
 * 'dstsel' respectively.
 * 
 * If 'srcsel' equals 'dstsel' and 'srci' equals 'dsti', then the repsective
 * location is to be deselected.
 */
struct transfer_t
{
	size_t srci;
	size_t dsti;
	enum selection_types srcsel;
	enum selection_types dstsel;
};

/**
 * Struct members that are pointers must be initialized before use.
 */
void f_init(struct freecell_t *f);

/**
 * Deals a new FreeCell game according to presets specified by the game type.
 */
void f_newgame(struct freecell_t *f, enum game_types gt);

/**
 * Performs a transfer on 'f' as specified by 't'.
 */
int f_transfer(struct freecell_t *f, struct transfer_t *t);

/**
 * Determines if card 'a' can be stacked on top of card 'b'.
 */
int can_stack(char a, char b);

#endif
