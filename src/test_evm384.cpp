// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <random>
#include "blst_evm384.h"

#define TEST_ITERATIONS 100000000

int compare_vec384(vec384 out_asm, vec384 out_no_asm, const char* func) {
    for (size_t j = 0; j < 6; ++j) {
      if (out_asm[j] != out_no_asm[j]) {
        std::cout << "ERROR - mismatch in " << func << std::endl;
        std::cout << "ASM:    0x" << std::setw(16) << std::hex
                  << out_asm[j] << std::endl;
        std::cout << "NO ASM: 0x" << std::setw(16) << std::hex
                  << out_no_asm[j] << std::endl;
        return -1;
      }
    }
    return 0;
}

int test_evm_384(size_t iters) {
  vec384 x, y; 
  vec384 out_asm, out_no_asm;

  std::mt19937_64 gen(1);\

  std::uniform_int_distribution<uint64_t>
    rng(0, std::numeric_limits<uint64_t>::max());

  // RNG for last limb to ensure scalar < order
  std::uniform_int_distribution<uint64_t>
    rng_upper(0, BLS12_381_P[5]);

  for (size_t i = 0; i < iters; ++i) {
    for (size_t k = 0; k < 5; ++k) {
      x[k] = rng(gen);
      y[k] = rng(gen);
    }
    x[5] = rng_upper(gen);
    y[5] = rng_upper(gen);

    add_mod_384(out_asm, x, y, BLS12_381_P);
    add_mod_384_no_asm(out_no_asm, x, y, BLS12_381_P);

    if (compare_vec384(out_asm, out_no_asm, "Add") != 0) {
      return -1;
    }

    sub_mod_384(out_asm, x, y, BLS12_381_P);
    sub_mod_384_no_asm(out_no_asm, x, y, BLS12_381_P);

    if (compare_vec384(out_asm, out_no_asm, "Sub") != 0) {
      return -1;
    }

    mul_mont_384(out_asm, x, y, BLS12_381_P, BLS12_381_p0);
    mul_mont_384_no_asm(out_no_asm, x, y, BLS12_381_P, BLS12_381_p0);

    if (compare_vec384(out_asm, out_no_asm, "Mul") != 0) {
      return -1;
    }
  }

  return 0;
}

int main() {
  std::cout << "Comparing " << TEST_ITERATIONS
            << " iterations of asm with no asm for add, sub, and mul"
            << std::endl;
  if (!test_evm_384(TEST_ITERATIONS)) {
    std::cout << "SUCCESS!" << std::endl;
  }
  return 0;
}
