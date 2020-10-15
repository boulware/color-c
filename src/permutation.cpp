#include "permutation.h"

#include "types.h"

u8 perm1[][1] = {
    {0},
};

u8 perm2[][2] = {
    {0,1},
    {1,0},
};

u8 perm3[][3] = {
    {0,1,2},
    {1,0,2},
    {2,0,1},
    {0,2,1},
    {1,2,0},
    {2,1,0},
};

u8 perm4[][4] = {
    {0,1,2,3},
    {1,0,2,3},
    {2,0,1,3},
    {0,2,1,3},
    {1,2,0,3},
    {2,1,0,3},
    {2,1,3,0},
    {1,2,3,0},
    {3,2,1,0},
    {2,3,1,0},
    {1,3,2,0},
    {3,1,2,0},
    {3,0,2,1},
    {0,3,2,1},
    {2,3,0,1},
    {3,2,0,1},
    {0,2,3,1},
    {2,0,3,1},
    {1,0,3,2},
    {0,1,3,2},
    {3,1,0,2},
    {1,3,0,2},
    {0,3,1,2},
    {3,0,1,2},
};

bool
GenerateU8Permutations(Array<u8> arrays, u8 *dst, size_t size_of_dst, u8 **cur_ptr)
{
    if(cur_ptr == nullptr)
    {
        u8 *cur = dst;
        cur_ptr = &cur;
    }


    if(arrays.count > 4)
    {
        Log(__FUNCTION__ "got an array with more than 4 sub-arrays. Permutations of more than 4 elements aren't supported.");
        return false;
    }
    if(arrays.count == 0)
    { // There are no permutations of 0 elements.
        return true;
    }
    else if(arrays.count == 1)
    {
        for(int p=0; p<ArrayCount(perm1); ++p) // iterate over each permutation
        {
            for(int i=0; i<arrays.count; ++i) // iterate over each element of the permutation
            {
                int index = perm1[p][i];
                *(*cur_ptr)++ = arrays[index];
            }
        }
    }
    else if(arrays.count == 2)
    {
        for(int p=0; p<ArrayCount(perm2); ++p) // iterate over each permutation
        {
            for(int i=0; i<arrays.count; ++i) // iterate over each element of the permutation
            {
                int index = perm2[p][i];
                *(*cur_ptr)++ = arrays[index];
            }
        }
    }
    else if(arrays.count == 3)
    {
        for(int p=0; p<ArrayCount(perm3); ++p) // iterate over each permutation
        {
            for(int i=0; i<arrays.count; ++i) // iterate over each element of the permutation
            {
                int index = perm3[p][i];
                *(*cur_ptr)++ = arrays[index];
            }
        }
    }
    else if(arrays.count == 4)
    {
        for(int p=0; p<ArrayCount(perm4); ++p) // iterate over each permutation
        {
            for(int i=0; i<arrays.count; ++i) // iterate over each element of the permutation
            {
                int index = perm4[p][i];
                *(*cur_ptr)++ = arrays[index];
            }
        }
    }

    return true;
}

// The arrays might be like this:
// (12)(3)(456) <=> [1,2], [3], [4,5,6]
// ...
// And this will generate basically "permutations of permutations".
// So... for the above example, we pick a number of from each sub-array and then
//       make all possible permutations of those numbers,
//       then pick another permutation of numbers from the sub-arrays and repeat.
//       And... do this for each possible selection of numbers from the sub-arrays
// The selections are (where order doesn't matter): ...
//     134, 135, 136, 234, 235, 236
// So we take each of these sets of numbers, and find all permutations for each set of them, add
// up the results, and put each unique permutation in the array exactly once in arbitrary order.

// [*cur] should generally only be set by recursive calls of this function.
// ... It's a way to track where in the [*dst] buffer we are currently at, so where
// the next permutation should be written to.
bool
GenerateU8Permutations(Array<Array<u8>> arrays, u8 *dst, size_t size_of_dst, u8 **cur_ptr)
{
    TIMED_BLOCK;

    if(cur_ptr == nullptr)
    {
        u8 *cur = dst;
        cur_ptr = &cur;
    }

    // Using the example in the above comments...
    // sub_permutations would contain 134, 135, 136, 234, 235, 236
    // ... and then we can pass those onto a function that takes a flat list to generate permutations
    int number_of_sub_permutations = m::Factorial(arrays.count);
    u8 *sub_permutations = nullptr;//(u8 *)malloc(arrays.count * number_of_sub_permutations);

    if(arrays.count > 4)
    {
        Log(__FUNCTION__ "got an array with more than 4 sub-arrays. Permutations of more than 4 elements aren't supported.");
        return false;
    }
    if(arrays.count == 0)
    { // There are no permutations of 0 elements.
        return true;
    }
    else if(arrays.count == 1) sub_permutations = (u8*)perm1;
    else if(arrays.count == 2) sub_permutations = (u8*)perm2;
    else if(arrays.count == 3) sub_permutations = (u8*)perm3;
    else if(arrays.count == 4) sub_permutations = (u8*)perm4;

    Array<u8> current_permutation = CreateTempArray<u8>(arrays.count);
    for(int i=0; i<arrays.count; ++i) current_permutation += (u8)0;

    int permutation_index = 0;
    while(permutation_index < number_of_sub_permutations)
    {
        for(int i=0; i<arrays.count; ++i)
        {
            u8 index = sub_permutations[arrays.count*permutation_index + i];
            current_permutation[i] = index;
        }

        if(arrays.count == 1)
        {
            for(int i=0; i<arrays[0].count; ++i)
            {
                *(*cur_ptr)++ = arrays[0][i];
            }
        }
        else if(arrays.count == 2)
        {
            // arrays[0][0] arrays
            for(int i=0; i<arrays[current_permutation[0]].count; ++i)
            {
                for(int j=0; j<arrays[current_permutation[1]].count; ++j)
                {
                    *(*cur_ptr)++ = arrays[current_permutation[0]][i];
                    *(*cur_ptr)++ = arrays[current_permutation[1]][j];
                }
            }
        }
        else if(arrays.count == 3)
        {
            for(int i=0; i<arrays[current_permutation[0]].count; ++i)
            {
                for(int j=0; j<arrays[current_permutation[1]].count; ++j)
                {
                    for(int k=0; k<arrays[current_permutation[2]].count; ++k)
                    {
                        *(*cur_ptr)++ = arrays[current_permutation[0]][i];
                        *(*cur_ptr)++ = arrays[current_permutation[1]][j];
                        *(*cur_ptr)++ = arrays[current_permutation[2]][k];
                    }
                }
            }
        }
        else if(arrays.count == 4)
        {
            for(int i=0; i<arrays[current_permutation[0]].count; ++i)
            {
                for(int j=0; j<arrays[current_permutation[1]].count; ++j)
                {
                    for(int k=0; k<arrays[current_permutation[2]].count; ++k)
                    {
                        for(int w=0; w<arrays[current_permutation[3]].count; ++w)
                        {
                            *(*cur_ptr)++ = arrays[current_permutation[0]][i];
                            *(*cur_ptr)++ = arrays[current_permutation[1]][j];
                            *(*cur_ptr)++ = arrays[current_permutation[2]][k];
                            *(*cur_ptr)++ = arrays[current_permutation[3]][w];
                        }
                    }
                }
            }
        }

        ++permutation_index;
    }

    //free(sub_permutations);

    return true;
}

