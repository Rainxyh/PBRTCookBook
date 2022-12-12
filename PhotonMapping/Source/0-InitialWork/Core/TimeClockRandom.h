#ifndef __TimeClockRandom_h__
#define __TimeClockRandom_h__

#include <stdlib.h>
#include <time.h>



inline void ClockRandomInit() {
	srand((unsigned)time(NULL));
}
inline double getClockRandom() {
	return rand() / (RAND_MAX + 1.0);
}







#endif




