#include <stdlib.h>

#include <dos.h>
#include <conio.h>

#include "cascade.h"
#include "extra.h"
#include "freecell.h"
#include "graphics.h"

#define DELAY_MS (250)
#define KING_WIDTH (32)
#define KING_HEIGHT (17)

static const unsigned char *const KING =
	"\x00\x00\x00\x00\x00\x00\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A"
	"\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\xA0\xA0\xAA\xA0\xAA\xA0\xAA\x00\xA1\x00\xA1\x00\xA1\x00\xA1"
	"\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x11\x01\x11\x01\x11\x10\x10\x10\x10\x10\x10\x10\x10\x10\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0E\x00\x0E"
	"\x00\x0E\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x0E\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xEE\x00\xEE\x00\xEE\x00\x00"
	"\x0F\x00\x00\xF0\x0F\x00\x00\x00\xE0\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\xEE\x00\xEE\x00\xEE\x00\xFF\xFF\xFF\xFF\x00"
	"\xFF\xFF\xFF\x00\xEE\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\xEE\x00\xEE\x00\xEE\x00\xFF\xF0\xFF\xF0\x00\xF0\xFF\xFF\x00"
	"\xEE\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xEE"
	"\x00\xEE\x00\xEE\x00\xFF\x0F\x0F\xF0\xF0\xF0\x0F\xFF\x00\xE0\x0E\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0E\xE0\xEE\x00\xEE\x00\xEE\x00\xEE"
	"\x00\xFF\xF0\xF0\xFF\xFF\xFF\xF0\xFF\x00\xEE\x00\xEE\xEE\x00\x00\x00\x00"
	"\x00\x00\x00\x0A\x00\xE0\x0E\x0E\xEE\xE0\x0E\xEE\x00\xEE\x00\x0E\xEE\x00"
	"\xEE\x00\xE0\x00\xEE\x0E\xE0\xE0\x00\x0A\xA0\x00\x0D\x00\xAA\x00\x10\xA0"
	"\x0A\x00\x00\x0A\x00\xE0\x00\x00\xE0\x00\xE0\xEE\x0E\xE0\x0E\xE0\x00\xAA"
	"\x00\x10\x00\x0A\xA0\x00\x0D\xD0\x00\x10\x00\xAA\x00\x01\xA0\x0A\x10\x01"
	"\x01\xA0\x00\xA0\x00\xA0\x00\x0A\x00\x0A\x00\x00\xAA\x00\x10\x00\xAA\x00"
	"\x00\xDD\x00\x11\x10\x10\x00\xA0\x0A\x00\x00\xAA\x00\x11\x11\x11\xAA\xA0"
	"\xA0\xA0\x0A\x0A\x01\x01\x00\xAA\x00\x10\x00\xAA\x00\x00\xDD\x00\x11\x10"
	"\x10\x10\xA0\x00\xAA\x00\x10\x00\xAA\x00\x10\x10\xA0\xA0\xA0\xA0\xA0\xA0"
	"\x10\x10\x0A\xA0\x01\x00\xAA\x00\x00\xDD\x00\x11\x10\x10\x10\x10\xA0\xA0"
	"\x00\xAA\x00\x00\xAA\x00\x0D\xDD\xD0\xDD\x0D\xDD\xD0\xDD\x0D\x00\xAA\x00"
	"\x00\x0A\xA0\x00\x0D\xD0\x01\x10\x0A\xA0\xA0\xA0\xA0\xA0\x00\xAA\x00\x10"
	"\x00\xAA\x00\xDD\x0D\xDD\x00\xDD\x0D\xDD\xD0\x00\xA0\x01\x00\xAA\x00\x00"
	"\xD0\x00\x11\x01\xA0\xA0\x00\xD0\xA0\xA0\xA0\x00\xA0\x00\x00\xA0\x00\xD0"
	"\x00\xD0\xD0\xD0\x00\xD0\x00\x00\x00\x00\x00\xA0\x00\xA0\x00\x00\x00\x10"
	"\x10\x10\x10\x10";

/**
 * Code shortener for transfer and update with delay.
 */
static bool transfer_update(struct game *g, const struct transfer *t);

/**
 * Determine how many cards, if any, need to be moved. The quota cannot be 0 if
 * the destination is empty. Otherwise, the maximum quota will be given.
 *
 * Consider the following scenarios (ignore colors):
 *
 * quota = 3: {K, Q, J, 10} -> {K}
 * quota = 4: {K, Q, J, 10} -> {}
 *
 * quota = 0: {K, Q, J, 9} -> {K}
 * quota = 1: {K, Q, J, 9} -> {}
 */
static size_t get_quota(struct cascade *src, struct cascade *dst);

static size_t n_tuple(struct game *g, const struct transfer *t, size_t quota);

/**
 * Prints out a picture of a king to the screen, using ASCII art. The top-left
 * of the picture will be at the specified (_x, _y) offset. The picture can be
 * flipped horizontally, if so desired.
 */
static void ascii_king(int _x, int _y, bool flip_h)
{
	unsigned char lo, hi;
	int x, y;
	size_t n;

	for (y = 0; y < KING_HEIGHT; y++)
	{
		gotoxy(_x, _y + y);

		for (x = 0; x < KING_WIDTH; x++)
		{
			n = flip_h ? KING_WIDTH * (y + 1) - (x + 1) : KING_WIDTH * y + x;
			lo = KING[n] & 0xF;
			hi = KING[n] >> 4;

			if (lo ^ hi)
			{
				textcolor(lo & 0x8 ? lo : hi);
				textbackground(lo & 0x8 ? hi : lo);
				cprintf(lo & 0x8 ? "\xDC" : "\xDF");
			}
			else
			{
				textcolor(lo);
				if (lo)
					cprintf("\xDB");
				else
					gotoxy(_x + x + 1, _y + y);
			}
		}
	}
}

static bool transfer_update(struct game *g, const struct transfer *t)
{
	bool b = f_transfer(g, t);
	update_display(g, t);
	delay(DELAY_MS);

	return b;
}

static size_t get_quota(struct cascade *src, struct cascade *dst)
{
	Card b;
	size_t i = src->size;

	while (--i && can_stack(src->cards[i], src->cards[i - 1]))
		continue;

	if (!isEmpty(dst)) /* TODO: camelCase or underscore_case ??? */
	{
		b = c_peek(dst);
		while (!can_stack(src->cards[i], b) && ++i < src->size)
			continue;
	}

	return src->size - i;
}

static size_t get_empty(size_t *arr, const struct game *g)
{
	size_t i, j;

	for (i = j = 0; i < g->num_cascades; i++)
	{
		if (isEmpty(g->cascades[i]))
			arr[j++] = i;
	}

	return j;
}

static size_t n_tuple(struct game *g, const struct transfer *t, size_t quota)
{
	size_t i, m;
	struct transfer T = *t;
	bool okay;

	m = g->num_freecells - g->freecells->size + 1;
	if (m > quota)
		m = quota;

	/* m - 1 cards from source to freecells */
	T.dstt = ST_FREECELL;
	for (i = 0; i < m - 1; i++)
		transfer_update(g, &T);

	/* 1 card from source to destination */
	T.dstt = ST_CASCADE;
	okay = transfer_update(g, &T);

	/* m - 1 cards from freecells to destination */
	T.srct = ST_FREECELL;
	T.srci = g->freecells->size - 1;
	if (!okay)
		T.dsti = t->srci; /* undo */
	for (i = 0; i < m - 1; i++, T.srci--)
		transfer_update(g, &T);

	return okay ? quota - m : quota;
}

/**
 * Recursive function for the transfer of multiple cards from the source
 * cascade to an empty cascade. Returns an updated quota for how many cards
 * still need to be moved.
 */
static size_t zippo(struct game *g, const struct transfer *t, size_t quota,
					size_t level)
{
	struct transfer T = *t;
	size_t empty[MAX_CASCADES], n, i;

	if (!quota)
		return 0;

	n = get_empty(empty, g);
	for (i = 0; i < n; i++)
	{
		if (empty[i] != t->dsti)
			break;
	}
	i = empty[i];

	if (!level || !n)
	{
		quota = n_tuple(g, &T, quota);
	}
	else
	{
		/* Source to temporary */
		T.dsti = i;
		quota = zippo(g, &T, quota, level - 1);

		/* Source to destination */
		T.dsti = t->dsti;
		quota = zippo(g, &T, quota, level - 1);

		/* Temporary to destination */
		T.srci = i;
		zippo(g, &T, g->cascades[i]->size, level - 1);
	}

	return quota;
}

bool auto_transfer(struct game *g, const struct transfer *t)
{
	size_t i, quota, m, n, empty[MAX_CASCADES];
	struct transfer T = *t;
	struct cascade *src, *dst;

	if (t->srct == ST_CASCADE && t->dstt == ST_CASCADE && t->srci != t->dsti)
	{
		src = g->cascades[t->srci];
		dst = g->cascades[t->dsti];

		if (isEmpty(src))
			return false;

		quota = get_quota(src, dst);
		if (!quota) /* no move is possible */
			return false;

		if (isEmpty(dst))
		{
			n_tuple(g, t, quota);

			return true;
		}

		/* Calculate m and n, which are used in formula */
		m = g->num_freecells - g->freecells->size + 1;
		if (m > quota)
			m = quota;
		n = get_empty(empty, g);

		/* Only use as many cascades as needed for efficiency */
		if (n)
		{
			while (m << n - 1 >= quota)
				n--;
		}

		/* Call recursive unloading method */
		quota -= m;
		for (i = 0; i < n; i++)
		{
			T.dsti = empty[i];
			if (quota)
				quota = zippo(g, &T, quota, n - i - 1);
		}
		quota += m;

		/* Source to destination */
		T.dsti = t->dsti;
		quota = n_tuple(g, &T, quota);

		/* Put everything else back */
		if (quota)
			T.dsti = t->srci;
		while (i--)
		{
			T.srci = empty[i];
			auto_transfer(g, &T);
		}

		return !quota;
	}
	else
		return f_transfer(g, t);
}
