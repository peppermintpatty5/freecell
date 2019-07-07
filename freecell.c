#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "conio.h"
#include "card.h"
#include "cascade.h"
#include "freecell.h"

static const char BORDER_TOP[] = {201, 205, 205, 205, 205, 205, 205, 187, 0},
                  BORDER_MID[] = {199, 196, 196, 196, 196, 196, 196, 182, 0},
                  BORDER_BOT[] = {200, 205, 205, 205, 205, 205, 205, 188, 0},
                  BORDER_L_R[] = {186, 32, 32, 32, 32, 32, 32, 186, 0};

static struct cascade_t *cascades[NUM_CASCADES];
static struct cascade_t *freecells;
static char homecells[NUM_HOMECELLS];

void hidecursor(void)
{
    gotoxy(1, 1);
}

void goto_freecell(int index)
{
    gotoxy(3, 4 + 4 * index);
}

void gotocc(char cascadei, char cardi)
{
    gotoxy(cascadei * 6 + 17, cardi + 3);
}

void pretty_borders(char x, char y)
{
    char i;

    textcolor(BLUE);
    textbackground(BLACK);

    for (i = 0; i < 17; i++)
    {
        gotoxy(x, i + y);
        if (i == 0)
            cprintf(BORDER_TOP);
        else if (i == 16)
            cprintf(BORDER_BOT);
        else if (i % 4 == 0)
            cprintf(BORDER_MID);
        else
            cprintf(BORDER_L_R);
    }
}

/**
 * TODO split up into smaller functions
 */
void refresh(void)
{
    char i, j;
    /* clear the screen */
    textcolor(BLACK);
    textbackground(BLACK);
    clrscr();
    pretty_borders(2, 2);
    pretty_borders(72, 2);
    /* numbers on the top of the screen */
    textcolor(LIGHTGRAY);
    gotoxy(5, 1);
    cprintf("0");
    gotoxy(75, 1);
    cprintf("9");
    for (i = 0; i < NUM_CASCADES; i++)
    {
        gotocc(i, -2);
        cprintf("  %d", i + 1);
    }
    /* print the cascades */
    for (i = 0; i < NUM_CASCADES; i++)
    {
        for (j = 0; j < cascades[i]->size; j++)
        {
            gotocc(i, j);
            cardprint(cascades[i]->cards[j], 0);
        }
    }
    refresh_freecells();
    refresh_homecells();
}

void refresh_freecells(void)
{
    char i;

    for (i = 0; i < NUM_FREECELLS; i++)
    {
        goto_freecell(i);
        if (i < freecells->size)
            cardprint(freecells->cards[i], 0);
        else
            carderase();
    }
}

void refresh_homecells(void)
{
    char i;

    for (i = 0; i < NUM_HOMECELLS; i++)
    {
        gotoxy(73, 4 + 4 * i);
        if (homecells[i] == NUM_CARDS)
        {
            textcolor(DARKGRAY);
            textbackground(BLACK);
            cprintf(" %c", i + '\3');
        }
        else
            cardprint(homecells[i], 0);
    }
}

void deal(void)
{
    unsigned char i, j, t;
    char deck[NUM_CARDS];
    /* shuffle */
    for (i = 0; i < NUM_CARDS; i++)
        deck[51 - i] = i;
    for (i = NUM_CARDS - 1; i > 0; i--)
    {
        j = rand() % i;
        t = deck[i];
        deck[i] = deck[j];
        deck[j] = t;
    }
    /* deal */
    for (i = 0; i < NUM_CARDS; i++)
        c_push(cascades[i % NUM_CASCADES], deck[i]);
}

void newgame(void)
{
    char i;
    /* clear and deal cascades */
    for (i = 0; i < NUM_CASCADES; i++)
        c_clr(cascades[i]);
    srand(time(NULL));
    for (i = 0; i < 1; i++)
        deal();
    /* clear freecells */
    c_clr(freecells);
    /* clear homecells */
    for (i = 0; i < NUM_HOMECELLS; i++)
        homecells[i] = NUM_CARDS;
}

int cascade_to_cascade(char srci, char dsti)
{
    struct cascade_t *src = cascades[srci],
                     *dst = cascades[dsti];
    char valid = dst->size == 0 ||
                 can_stack(c_peek(src), c_peek(dst));

    if (valid)
    {
        gotocc(srci, src->size - 1);
        carderase();
        c_push(dst, c_pop(src));
        gotocc(dsti, dst->size - 1);
        cardprint(c_peek(dst), 0);
    }

    return valid;
}

int can_stack(char a, char b)
{
    return getrank(b) - getrank(a) == 1 &&
           isblack(a) != isblack(b);
}

int freecell_to_cascade(char srci, char dsti)
{
    struct cascade_t *dst = cascades[dsti];
    char valid = dst->size == 0 ||
                 can_stack(freecells->cards[srci], c_peek(dst));

    if (valid)
    {
        c_push(dst, c_rm(freecells, srci));
        gotocc(dsti, dst->size - 1);
        cardprint(c_peek(dst), 0);
        refresh_freecells();
    }

    return valid;
}

int cascade_to_freecell(char srci)
{
    struct cascade_t *src = cascades[srci];
    char valid = freecells->size < NUM_FREECELLS;

    if (valid)
    {
        c_push(freecells, c_pop(src));
        gotocc(srci, src->size);
        carderase();
        refresh_freecells();
    }

    return valid;
}

int to_homecell(char srci, enum selection_types selection)
{
    char a, b, valid;

    switch (selection)
    {
    case SELECT_FREECELL:
        a = freecells->cards[srci];
        break;
    case SELECT_CASCADE:
        a = c_peek(cascades[srci]);
        break;
    }

    b = homecells[getsuit(a)];
    valid = (getrank(a) == 0 && b == NUM_CARDS) ||
            (getrank(a) - getrank(b) == 1);

    if (valid)
    {
        switch (selection)
        {
        case SELECT_FREECELL:
            homecells[getsuit(a)] = c_rm(freecells, srci);
            refresh_freecells();
            break;

        default:
            homecells[getsuit(a)] = c_pop(cascades[srci]);
            gotocc(srci, cascades[srci]->size);
            carderase();
            break;
        }
        refresh_homecells();
    }

    return valid;
}

void init(void)
{
    char i;

    directvideo = 0;
    textmode(C80);

    for (i = 0; i < NUM_CASCADES; i++)
        cascades[i] = cascade_new(MAX_CASCADE_SIZE);
    freecells = cascade_new(NUM_FREECELLS);
}

int main(void)
{
    enum selection_types selection = SELECT_NONE;
    signed int srci, i, key;

    init();
    newgame();
    refresh();

    do
    {
        hidecursor();
        key = getch();
        switch (key)
        {
        case 'q':
        case 'Q':
            gotoxy(1, 25);
            textcolor(LIGHTGREEN);
            textbackground(BLACK);
            cprintf("Press 'q' again to confirm exit.");
            hidecursor();
            key = getch();
            if (key == 'q' || key == 'Q')
                goto RET;
            else
            {
                gotoxy(1, 25);
                delline();
            }
            break;
        case 'n':
        case 'N':
            /* TODO shorten rendunant confirmation code */
            gotoxy(1, 25);
            textcolor(LIGHTGREEN);
            textbackground(BLACK);
            cprintf("Press 'n' again to confirm new game.");
            hidecursor();
            key = getch();
            if (key == 'n' || key == 'N')
            {
                selection = SELECT_NONE;
                newgame();
                refresh();
            }
            else
            {
                gotoxy(1, 25);
                delline();
            }
            break;
        case '0':
            switch (selection)
            {
            case SELECT_NONE:
                /* cannot select empty freecells */
                if (freecells->size)
                {
                    selection = SELECT_FREECELL;
                    srci = freecells->size - 1;
                    goto_freecell(srci);
                    cardprint(freecells->cards[srci], 1);
                }
                break;
            case SELECT_FREECELL:
                goto_freecell(srci);
                cardprint(freecells->cards[srci], 0);
                srci--;
                if (srci < 0)
                    selection = SELECT_NONE;
                else
                {
                    goto_freecell(srci);
                    cardprint(freecells->cards[srci], 1);
                }
                break;
            case SELECT_CASCADE:
                if (cascade_to_freecell(srci))
                    selection = SELECT_NONE;
                break;
            }
            break;
        case '9':
            switch (selection)
            {
            case SELECT_NONE:
                /* cannot select the homecells */
                break;
            case SELECT_FREECELL:
                if (to_homecell(srci, SELECT_FREECELL))
                    selection = SELECT_NONE;
                break;
            case SELECT_CASCADE:
                if (to_homecell(srci, SELECT_CASCADE))
                    selection = SELECT_NONE;
                break;
            }
            break;
        default:
            i = key - '1';
            if (i >= 0 && i < NUM_CASCADES)
            {
                switch (selection)
                {
                case SELECT_NONE:
                    /* cannot select empty cascade */
                    if (cascades[i]->size)
                    {
                        selection = SELECT_CASCADE;
                        srci = i;
                        gotocc(srci, cascades[srci]->size - 1);
                        cardprint(c_peek(cascades[srci]), 1);
                    }
                    break;
                case SELECT_FREECELL:
                    if (freecell_to_cascade(srci, i))
                        selection = SELECT_NONE;
                    break;
                case SELECT_CASCADE:
                    if (i == srci)
                    {
                        gotocc(srci, cascades[srci]->size - 1);
                        cardprint(c_peek(cascades[srci]), 0);
                        selection = SELECT_NONE;
                    }
                    else if (cascade_to_cascade(srci, i))
                        selection = SELECT_NONE;
                    break;
                }
            }
            break;
        }
    } while (1);

RET:
    textbackground(BLACK);
    textcolor(LIGHTGRAY);
    clrscr();
    return EXIT_SUCCESS;
}
