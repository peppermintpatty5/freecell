#ifndef _FREECELL_H_
#define _FREECELL_H_

#include <stddef.h>

#include "bool.h"
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

enum gametype
{
	SINGLE_DECK,
	DOUBLE_DECK
};

enum selection
{
	ST_NONE,
	ST_CASCADE,
	ST_FREECELL,
	ST_HOMECELL
};

/**
 * Contains the data for a game of FreeCell. Supports both single and double
 * deck FreeCell variations.
 */
struct game
{
	size_t num_decks;
	size_t num_cascades;
	size_t num_freecells;
	struct cascade *cascades[MAX_CASCADES];
	struct cascade *freecells;
	Card homecells[NUM_SUITS * MAX_DECKS];
};

/**
 * Contains the data for transferring a single card. 'scri' and 'dsti' are the
 * source/destitnation indices of the locations specified by 'srct' and
 * 'dstt' respectively.
 *
 * If 'srct' equals 'dstt' and 'srci' equals 'dsti', then the repsective
 * location is to be deselected.
 */
struct transfer
{
	/* TODO: replace with unions? which may be more elegant */
	size_t srci;
	size_t dsti;
	enum selection srct;
	enum selection dstt;
};

/**
 * Struct members that are pointers must be initialized before use.
 */
void f_init(struct game *g);

/**
 * Deals a new FreeCell game according to presets specified by the game type.
 */
void f_newgame(struct game *g, enum gametype gt);

/**
 * Performs a transfer on 'f' as specified by 't'.
 */
bool f_transfer(struct game *g, const struct transfer *t);

#endif
