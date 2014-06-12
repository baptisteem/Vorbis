#include "helpers.h"
#include <math.h>
#include <stdio.h>

uint32_t ilog(int32_t val){

	uint32_t i=0;
	while (val > 0){
		val /= 2;
		i ++;
	}
	return i;
}


uint32_t lookup1_values(uint32_t a, uint32_t b){
	
	uint32_t r=0;
	sample_t res= pow((sample_t)r,(sample_t)b);

	while (res <= (sample_t)a){
		r++;
		res = pow((sample_t)r,(sample_t)b);
	}

	return (r-1);
}

sample_t float32_unpack(uint32_t packed){
	
	uint32_t mantissa= packed & 0x001fffff;
	uint32_t sign= packed & 0x80000000;
	uint32_t exponent= (packed & 0x7fe00000) >> 21;
	int32_t mantisse=(int32_t)mantissa;
	
	if (sign != 0){
		mantisse = -mantisse;
	}

	sample_t res=(sample_t) exponent;
	res -= 788.0;
	res =pow(2.0,res);
	res *= (sample_t) mantisse;

	return res;
}
