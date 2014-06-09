#include "helpers.h"
#include <math.h>
#include <stdio.h>

uint32_t ilog(int32_t val){

	uint32_t i=0;

	if (val <= 0){
		return 0;
	}
	else{
		while (val != 0){
			val /= 2;
			i ++;
		}
		return i;
	}
}

uint32_t lookup1_values(uint32_t a, uint32_t b){
	
	uint32_t r=0;
	uint32_t res=(uint32_t) pow((sample_t)r,(sample_t)b);

	while (res <= a){
		r++;
		res = (uint32_t) pow((sample_t)r,(sample_t)b);
	}
	
	if (r==0){
		//send an error: there is no integer r such as
		//r^b <= a
	}
	else{
		return (r-1);
	}
}

sample_t float32_unpack(uint32_t packed){
	
	int32_t mantissa= packed & 0x001fffff;
	int32_t sign= packed & 0x80000000;
	uint32_t exponent= ((packed & 0x7fe00000) >> 21);
	
	if (sign != 0){
		mantissa = -mantissa;
	}
	
	sample_t aux= pow((sample_t)2,(sample_t)(exponent-788));
	return (aux*(sample_t)mantissa);
}
