#include "utils.h"

#define N 10000

/*!
 *  @brief  Delays the processor for milli milliseconds
 *  @param  milli
 *          the number of milliseconds to delay
 */

void utils::delay(int milli) {
	int i;
	int j;
	for(i=0; i<milli; i++) {
		for(j=0; j<N; j++) {
			__asm("nop"); 
		}	
	}
}