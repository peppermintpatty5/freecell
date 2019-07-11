#ifndef _FREECELL_H_
#define _FREECELL_H_

#define NUM_DECKS (1)
#define NUM_CASCADES (10)
#define NUM_FREECELLS (8)
#define NUM_HOMECELLS (8)
#define MAX_CASCADE_SIZE (24)

enum selection_types
{
    SELECT_NONE,
    SELECT_FREECELL,
    SELECT_CASCADE
};

void hidecursor(void);

/**
 * Displays confirmation message on bottom line of screen and waits for user to
 * type 'y' or 'n'. The line is erased once either choice has been made. Return
 * values are non-zero for 'yes' and zero for 'no'.
 */
int confirm_yn(char *message);

/**
 * Move the cursor to the nth freecell.
 */
void goto_freecell(int index);

void gotocc(char cascadei, char cardi);

void pretty_borders(char x, char y);

void refresh(void);

void refresh_freecells(void);

void refresh_homecells(void);

/**
 * Deal n many shuffled decks to the cascades.
 */
void deal(unsigned int n);

void newgame(void);

/**
 * Determines if card a can be stacked on top of card b.
 */
int can_stack(char a, char b);

/**
 * Attempt to transfer card from one cascade to another, according to the rules
 * of FreeCell. Returns 0 if failed, non-zero if succeeded.
 * 
 * Destination index can refer to empty cascade, while source index cannot.
 * 
 * Also performs graphical updates (hopefully).
 */
int cascade_to_cascade(int srci, int dsti);

/**
 * A (probably) recursive function that moves multiple cards from one stack to
 * another.
 * 
 * Destination index can refer to empty cascade, while source index cannot.
 * 
 * Also does cool animation if you want to see it.
 */
int cascade_to_cascade_m(int srci, int dsti);

int freecell_to_cascade(int srci, int dsti);

int cascade_to_freecell(int srci);

/**
 * Attemps to move card from a freecell or cascade to the homecells.
 */
int to_home(char srci, enum selection_types selection);

/**
 * Initializes static variables and sets up display. Only call once!
 */
void init(void);

int main(void);

#endif
