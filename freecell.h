#ifndef _FREECELL_H_
#define _FREECELL_H_

#define NUM_CASCADES 8
#define NUM_FREECELLS 4
#define NUM_HOMECELLS 4
#define MAX_CASCADE_SIZE 20

enum selection_types
{
    SELECT_NONE,
    SELECT_FREECELL,
    SELECT_CASCADE
};

void hidecursor(void);

/**
 * Move the cursor to the nth freecell.
 */
void goto_freecell(int index);

void gotocc(char cascadei, char cardi);

void pretty_borders(char x, char y);

void refresh(void);

void refresh_freecells(void);

void refresh_homecells(void);

void deal(void);

void newgame(void);

/**
 * Attempt to transfer card from one cascade to another, according to the rules
 * of FreeCell. Returns 0 if failed, non-zero if succeeded.
 * 
 * Destination index can refer to empty cascade, while source index cannot.
 * 
 * Also performs graphical updates (hopefully).
 */
int cascade_to_cascade(char srci, char dsti);

/**
 * Determines if card a can be stacked on top of card b.
 */
int can_stack(char a, char b);

int freecell_to_cascade(char srci, char dsti);

int cascade_to_freecell(char srci);

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
