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

typedef enum
{
	SINGLE_DECK,
	DOUBLE_DECK
} GameType;

typedef enum
{
	ST_NONE,
	ST_CASCADE,
	ST_FREECELL,
	ST_HOMECELL
} SelectType;

/**
 * Contains the data for a game of FreeCell. Supports both single and double
 * deck FreeCell variations.
 */
typedef struct
{
	size_t num_decks;
	size_t num_cascades;
	size_t num_freecells;
	Cascade *cascades[MAX_CASCADES];
	Cascade *freecells;
	Card homecells[NUM_SUITS * MAX_DECKS];
} FreeCell;

/**
 * Contains the data for transferring a single card. 'scri' and 'dsti' are the
 * source/destitnation indices of the locations specified by 'srct' and
 * 'dstt' respectively.
 *
 * If 'srct' equals 'dstt' and 'srci' equals 'dsti', then the repsective
 * location is to be deselected.
 */
typedef struct
{
	/* TODO: replace with unions? which may be more elegant */
	size_t srci;
	size_t dsti;
	SelectType srct;
	SelectType dstt;
} Transfer;

/**
 * Struct members that are pointers must be initialized before use.
 */
void f_init(FreeCell *f);

/**
 * Deals a new FreeCell game according to presets specified by the game type.
 */
void f_newgame(FreeCell *f, GameType gt);

/**
 * Performs a transfer on 'f' as specified by 't'.
 */
bool f_transfer(FreeCell *f, const Transfer *t);

#endif
