// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>
#include "blst_evm384.h"

void add_mod_384_no_asm(vec384 ret, const vec384 a, const vec384 b,
                        const vec384 p) {
  __uint128_t limbx;
  uint64_t mask, carry, borrow;
  vec384 tmp;
  std::size_t i;

  for (carry=0, i=0; i<6; i++) {
    limbx = a[i] + (b[i] + (__uint128_t)carry);
    tmp[i] = (uint64_t)limbx;
    carry = (uint64_t)(limbx >> 64);
  }

  for (borrow=0, i=0; i<6; i++) {
    limbx = tmp[i] - (p[i] + (__uint128_t)borrow);
    ret[i] = (uint64_t)limbx;
    borrow = (uint64_t)(limbx >> 64) & 1;
  }

  mask = carry - borrow;

  for(i=0; i<6; i++)
    ret[i] = (ret[i] & ~mask) | (tmp[i] & mask);
}

void sub_mod_384_no_asm(vec384 ret, const vec384 a, const vec384 b,
                        const vec384 p) {
  __uint128_t limbx;
  uint64_t mask, carry, borrow;
  std::size_t i;

  for (borrow=0, i=0; i<6; i++) {
    limbx = a[i] - (b[i] + (__uint128_t)borrow);
    ret[i] = (uint64_t)limbx;
    borrow = (uint64_t)(limbx >> 64) & 1;
  }

  mask = 0 - borrow;

  for (carry=0, i=0; i<6; i++) {
    limbx = ret[i] + ((p[i] & mask) + (__uint128_t)carry);
    ret[i] = (uint64_t)limbx;
    carry = (uint64_t)(limbx >> 64);
  }
}

void mul_mont_384_no_asm(vec384 ret, const vec384 a, const vec384 b,
                         const vec384 p, uint64_t n0) {
  __uint128_t limbx;
  uint64_t mask, borrow, mx, hi, tmp[7], carry;
  std::size_t i, j;

  for (mx=b[0], hi=0, i=0; i<6; i++) {
    limbx = (mx * (__uint128_t)a[i]) + hi;
    tmp[i] = (uint64_t)limbx;
    hi = (uint64_t)(limbx >> 64);
  }
  mx = n0*tmp[0];
  tmp[i] = hi;

  for (carry=0, j=0; ; ) {
    limbx = (mx * (__uint128_t)p[0]) + tmp[0];
    hi = (uint64_t)(limbx >> 64);
    for (i=1; i<6; i++) {
      limbx = (mx * (__uint128_t)p[i] + hi) + tmp[i];
      tmp[i-1] = (uint64_t)limbx;
      hi = (uint64_t)(limbx >> 64);
    }
    limbx = tmp[i] + (hi + (__uint128_t)carry);
    tmp[i-1] = (uint64_t)limbx;
    carry = (uint64_t)(limbx >> 64);

    if (++j==6)
      break;

    for (mx=b[j], hi=0, i=0; i<6; i++) {
      limbx = (mx * (__uint128_t)a[i] + hi) + tmp[i];
      tmp[i] = (uint64_t)limbx;
      hi = (uint64_t)(limbx >> 64);
    }
    mx = n0*tmp[0];
    limbx = hi + (__uint128_t)carry;
    tmp[i] = (uint64_t)limbx;
    carry = (uint64_t)(limbx >> 64);
  }

  for (borrow=0, i=0; i<6; i++) {
    limbx = tmp[i] - (p[i] + (__uint128_t)borrow);
    ret[i] = (uint64_t)limbx;
    borrow = (uint64_t)(limbx >> 64) & 1;
  }

  mask = carry - borrow;

  for(i=0; i<6; i++)
    ret[i] = (ret[i] & ~mask) | (tmp[i] & mask);
}

