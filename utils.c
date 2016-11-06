#include <stdlib.h>

#include "utils.h"

void freeAndNull(void **p)
{
	free(*p);
	*p = NULL;
}

bool isEven(uint32_t x)
{
	return !isOdd(x);
}

bool isOdd(uint32_t x)
{
	return (x & 1);
}

