#ifndef _MAIN_H_
#define _MAIN_H_

#define hidecursor gotoxy(1, 1)

/**
 * Displays confirmation message on bottom line of screen and waits for user to
 * type 'y' or 'n'. The line is erased once either choice has been made. Return
 * values are non-zero for 'yes' and zero for 'no'.
 */
int confirm_yn(const char *message);

void goto_cascade(int, int);

void refresh(void);

void refresh_cascade_tail(size_t index, int select);

void refresh_freecells(void);

void refresh_homecells(void);

/**
 * Initializes static variables and sets up display. Only call once!
 */
void init(void);

/**
 * Program point of entry.
 */
int main(void);

#endif
