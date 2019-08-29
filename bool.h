#if __STDC_VERSION__ >= 199901L /* C99 check */

#include <stdbool.h>

#else

#ifndef _BOOL_H
#define _BOOL_H

typedef enum
{
	false = 0,
	true = 1
} bool;

#endif

#endif
