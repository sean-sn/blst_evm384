// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef __BLST_EVM384_H__
#define __BLST_EVM384_H__

#if defined(__ADX__) /* e.g. -march=broadwell */ && !defined(__BLST_PORTABLE__)
# define mul_mont_384 mulx_mont_384
#endif

typedef uint64_t vec384[6];

extern "C" {
  void add_mod_384(vec384 ret, const vec384 a, const vec384 b, const vec384 p);
  void sub_mod_384(vec384 ret, const vec384 a, const vec384 b, const vec384 p);
  void mul_mont_384(vec384 ret, const vec384 a, const vec384 b,
                    const vec384 p, uint64_t n0);
}

// BLS12-381 Modulus
const uint64_t BLS12_381_P[6] = {
    0xb9feffffffffaaab, 0x1eabfffeb153ffff,
    0x6730d2a0f6b0f624, 0x64774b84f38512bf,
    0x4b1ba7b6434bacd7, 0x1a0111ea397fe69a
};

const uint64_t BLS12_381_p0 = (uint64_t)0x89f3fffcfffcfffd;  /* -1/P */

void add_mod_384_no_asm(vec384 ret, const vec384 a, const vec384 b,
                        const vec384 p);
void sub_mod_384_no_asm(vec384 ret, const vec384 a, const vec384 b,
                        const vec384 p);
void mul_mont_384_no_asm(vec384 ret, const vec384 a, const vec384 b,
                         const vec384 p, uint64_t n0);

#endif
