#include "math.h"
#include <immintrin.h>

#include "debug.h"

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

    float Round(float value)
    {
        return((float)(int)(value + 0.5f));
    }

    template <class Type>
    Type Pow(Type base, int power)
    {
        Type product = (Type)1;
        if(power == 0) {}
        else if(power > 0)
        {
            for(int i=0; i<power; ++i)
            {
                product *= base;
            }
        }
        else
        { // negative power
            for(int i=0; i<-power; ++i)
            {
                product /= base;
            }
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

    float Sin(float a)
    {
        return _mm_cvtss_f32(_mm_sin_ps(_mm_set_ss(a)));
    }

    float Cos(float a)
    {
        return _mm_cvtss_f32(_mm_cos_ps(_mm_set_ss(a)));
    }

    float Lerp(float start, float end, float t)
    {
        float value = (1.f - t)*start + (t)*end;
        return value;
    }

    s64
    Factorial(s64 a)
    {
        TIMED_BLOCK;
        s64 value = 1;
        for(int i=a; i>1; --i)
        {
            value *= i;
        }

        return value;
    }
};