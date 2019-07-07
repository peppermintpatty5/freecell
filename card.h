#ifndef _CARD_H_
#define _CARD_H_

#define NUM_CARDS 52

/**
 * Composes a byte (char) through bitshift + OR. Bits 0-1 are suit, bits 2-5
 * are rank, and bits 6-7 are unused.
 * 
 * Rank shall be from 0-12 and suit shall be from 0-3.
 */
#define getcard(rank, suit) (((rank) << 2) | (suit))
#define getrank(card) ((card) >> 2)
#define getsuit(card) ((card) & (0x3))
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
