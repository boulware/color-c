#ifndef PERMUTATION_H
#define PERMUTATION_H

bool GenerateU8Permutations(Array<Array<u8>> arrays, u8 *dst, size_t size_of_dst, u8 **cur = nullptr);
bool GenerateU8Permutations(Array<u8> arrays, u8 *dst, size_t size_of_dst, u8 **cur = nullptr);

#endif