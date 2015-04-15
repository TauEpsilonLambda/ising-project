#include <gsl/gsl_rng.h>

gsl_rng* rng = gsl_rng_alloc( gsl_rng_default );

//the pow function annoyingly only gives doubles. To avoid having to constantly recast to ints, I made my own function. Requires non-negative exponent.
int power(int base, unsigned int exponent) {
	if (exponent==0){return 1;}
	else {return base*power(base,exponent-1);}
}