#ifndef MATH_H
#define MATH_H

namespace m
{
	template <class Type>
	Type Min(Type a, Type b);

	template <class Type>
	Type Max(Type a, Type b);

	template <class Type>
	Type Clamp(Type value, Type low, Type high);

	template <class Type>
	Type Abs(Type value);

	template <class Type>
	Type Pow(Type base, unsigned int power);

	float Sqrt(float a);
	float Round(float value);
};

#endif