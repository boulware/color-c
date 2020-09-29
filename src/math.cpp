#include "math.h"
#include <immintrin.h>

namespace m
{
	template <class Type>
	Type Min(Type a, Type b) {
		if(a <= b) return a;
		else return b;
	}

	template <class Type>
	Type Max(Type a, Type b) {
		if(a >= b) return a;
		else return b;
	}

	template <class Type>
	Type Clamp(Type value, Type low, Type high) {
		return Min(Max(value, low), high);
	}

	template <class Type>
	Type Abs(Type value) {
		if(value >= 0) return value;
		else return -value;
	}

	template <class Type>
	Type Pow(Type base, unsigned int power)
	{
		Type product = 1;
		for(int i=0; i<power; i++)
		{
			product *= base;
		}
		return product;
	}

	template <class Type>
	int Sign(Type value)
	{
		if(value < (Type)0) return -1;
		else if(value > (Type)0) return +1;

		return 0;
	}

	float Sqrt(float a)
	{
		return _mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(a)));
	}
};