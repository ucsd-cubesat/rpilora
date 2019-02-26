/*
 * helper.c
 *
 *  Created on: Feb 17, 2019
 *      Author: TritonCubed
 *
 *  Helper functions, readily available
 *  across the program.
 */

#include <time.h>

/**
 * Helper function to delay execution.
 * Duration must be given in seconds.
 * Example:
 *   time_delay( 0.5 ) - delays the execution by
 *                       half a second
 */
void time_delay( double duration ) {

  //saves the duration since invocation
  clock_t timer = 0;

  //saves the time of invocation
  clock_t start = clock();

  //while the time since invocation is less than the desired duration...
  while( ( timer / CLOCKS_PER_SEC ) < duration ) {
    //...refresh the duration
    timer = clock() - start;
  }
}
