#include "filters.h"

int32_t LowPassFilter(int32_t *input, int32_t *intermediate, uint8_t shifts)
{
	*intermediate = (*intermediate - (*intermediate >> shifts)) + *input;
	return (*intermediate >> shifts);
}
