#ifndef _CARD_H_
#define _CARD_H_

#include <stddef.h>

#define NUM_RANKS (13)
#define NUM_SUITS (4)

/**
 * This value is additionally used for empty homecells, since it falls outside
 * the valid range of 0-51.
 */
#define NUM_CARDS (NUM_RANKS * NUM_SUITS)

/**
 * Composes a byte (char) through bitshift + OR. Bits 0-1 are suit, bits 2-5
 * are rank, and bits 6-7 are unused. Rank shall be from 0-12 and suit shall be
 * from 0-3. Macro is used mainly for debugging.
 */
#define getcard(rank, suit) (((rank) << 2) | (suit))

/**
 * Extracts the rank (bits 2-5) from a byte (char) value.
 */
#define getrank(card) ((card) >> 2)

/**
 * Extracts the suit (bits 0-1) from a byte (char) value.
 */
#define getsuit(card) ((card) & (0x3))

/**
 * Extracts the color (bit 1) from the suit, where 1 is black.
 */
#define isblack(card) (getsuit(card) >> 1)

/**
 * Prints a card at the current location of the cursor.
 */
void cardprint(char card, char select);

/**
 * Prints 5 spaces at the current location of the cursor.
 */
void carderase(void);

#endif
