/* code borrowed from ftp://ftp.taygeta.com/pub/c/boxmuller.c */
/* boxmuller.c           Implements the Polar form of the Box-Muller
                         Transformation

                      (c) Copyright 1994, Everett F. Carter Jr.
                          Permission is granted by the author to use
			  this software for any application provided this
			  copyright notice is preserved.

*/
#include <math.h>
#include <stdlib.h>
#include "boxmuller.h"

#if defined(_MSC_VER)
#define drand48() ( (double) (rand() / RAND_MAX) )
//#define drand48() rand()
#endif

/*
change:
	http://bytes.com/topic/c/answers/136343-can-i-replace-drand48-srand48-random
	replace srand48(seed) by srand(seed)
	replace drand48() by (double(rand()) / RAND_MAX)
*/

double box_muller(double m, double s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	double x1, x2, w, _y1;
	static double y2;
	static int use_last = 0;
	double result;

	if (use_last)		        /* use value from previous call */
	{
		_y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * drand48() - 1.0;
			x2 = 2.0 * drand48() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		_y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	result = ( m + _y1 * s );

	return result;
}
