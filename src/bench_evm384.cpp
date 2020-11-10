// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

// Prints results in format that benchstat can process
//#define PRINT_GO_BENCHSTAT_FORMAT true
#define PRINT_GO_BENCHSTAT_FORMAT false

// Use define to bench functions using only serial dependence between calls
// This is the proper method.  Not defining is simply to show the difference 
//   between method results.
#define BENCH_ONLY_SERIAL_DEPENDENCE true

// Prints verbose data about each benchmark result
#define PRINT_BENCH_VERBOSE true

#include <iostream>
#include <iomanip>
#include <locale>
#include <vector>
#include <math.h>
#include <chrono>
#include <ctime>
#include <random>
#include <cstring>


#include "bench.h"
#include "blst_evm384.h"

// Outer iterations are number of bench runs to perform per function
// Inner iterations are the number of times to run the function in a timed loop
#define OUTER_ITERS_FAST 10
#define INNER_ITERS_FAST 1000000

BENCH_FUNC(OUTER_ITERS_FAST, INNER_ITERS_FAST, EVM384AddBLS381, add_mod_384,
           dest, x, y, BLS12_381_P)

BENCH_FUNC(OUTER_ITERS_FAST, INNER_ITERS_FAST, EVM384AddNoAsmBLS381,
           add_mod_384_no_asm, dest, x, y, BLS12_381_P)

BENCH_FUNC(OUTER_ITERS_FAST, INNER_ITERS_FAST, EVM384SubBLS381, sub_mod_384,
           dest, x, y, BLS12_381_P)

BENCH_FUNC(OUTER_ITERS_FAST, INNER_ITERS_FAST, EVM384SubNoAsmBLS381,
           sub_mod_384_no_asm, dest, x, y, BLS12_381_P)

BENCH_FUNC(OUTER_ITERS_FAST, INNER_ITERS_FAST, EVM384MulBLS381, mul_mont_384,
           dest, x, y, BLS12_381_P, BLS12_381_p0)

BENCH_FUNC(OUTER_ITERS_FAST, INNER_ITERS_FAST, EVM384MulNoAsmBLS381,
           mul_mont_384_no_asm, dest, x, y, BLS12_381_P, BLS12_381_p0)

int main(int argc, char **argv) {
  bool skip_cycle_check = false;
  if (argc > 1) {
    if (!strcmp("-skip-cycle-check", argv[1])) {
      skip_cycle_check = true;
    }
  }

  Perf perf(OUTER_ITERS_FAST, INNER_ITERS_FAST);

  // Check for stable CPU clock frequency
  uint64_t  cycles_per_sec = perf.get_cycles_per_sec();

  if (cycles_per_sec == 0) {
    if (skip_cycle_check) {
      std::cout << "Unstable frequency!! Proceeding anyway" << std::endl;
    } else {
      std::cout << "Skipping benchmark runs - unstable frequency" << std::endl;
      return -1;
    }
  }

  std::cout.imbue(std::locale(""));
  std::cout << "CPU cyc/sec: " << std::fixed << cycles_per_sec << std::endl;
  std::cout.imbue(std::locale());
  std::cout << std::endl;

  std::cout << "Benchmarking with parameters" << std::endl;
  std::cout << "OUTER_ITERS_FAST: " << OUTER_ITERS_FAST << std::endl;
  std::cout << "INNER_ITERS_FAST: " << INNER_ITERS_FAST << std::endl;

  std::cout << std::endl;
  std::cout << "Compiler:         ";
#ifdef __INTEL_COMPILER
  std::cout << "Intel " << std::setprecision(1)
            << (double)(__INTEL_COMPILER / 100) << std::endl;
#elif __clang__
  std::cout << "clang " << __clang_version__ << std::endl;
#elif __GNUC__
  std::cout << "GNU " << __GNUC__ << "." << __GNUC_MINOR__ << std::endl;
#else
  std::cout << "UNKNOWN" << std::endl;
#endif

  std::uniform_int_distribution<uint64_t> 
    dist(0, std::numeric_limits<uint64_t>::max());

  // RNG for last limb to ensure scalar < order
  std::uniform_int_distribution<uint64_t> 
    dist_upper(0, BLS12_381_P[5]);


  auto startClock = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(startClock);
  std::cout << "Run date: " << std::ctime(&startTime) << std::endl;

  /*
  std::vector<bench_func_ptr_t> benches = {
    BenchEVM384AddBLS381Same,
    BenchEVM384AddBLS381Diff,
    BenchEVM384AddNoAsmBLS381Same,
    BenchEVM384AddNoAsmBLS381Diff,
    BenchEVM384SubBLS381Same,
    BenchEVM384SubBLS381Diff,
    BenchEVM384SubNoAsmBLS381Same,
    BenchEVM384SubNoAsmBLS381Diff,
    BenchEVM384MulBLS381Same,
    BenchEVM384MulBLS381Diff,
    BenchEVM384MulNoAsmBLS381Same,
    BenchEVM384MulNoAsmBLS381Diff,
  };
  */
  std::vector<bench_func_ptr_t> benches;
  ADD_BENCH_FUNC(EVM384AddBLS381, benches);
  ADD_BENCH_FUNC(EVM384AddNoAsmBLS381, benches);
  ADD_BENCH_FUNC(EVM384SubBLS381, benches);
  ADD_BENCH_FUNC(EVM384SubNoAsmBLS381, benches);
  ADD_BENCH_FUNC(EVM384MulBLS381, benches);
  ADD_BENCH_FUNC(EVM384MulNoAsmBLS381, benches);

  std::vector<BenchResult> results;

  for (auto it = benches.begin(); it != benches.end(); ++it) {
    if(*it != 0) {
      (*it)(&perf, dist, dist_upper, results);
    } else {
      BenchResult result;
      result.placeholder = true;
      result.name = "_";
      results.push_back(result);
    }
  };

#ifdef PRINT_BENCH_VERBOSE
  std::cout.imbue(std::locale(""));
  std::cout << std::endl;
  for (auto it = results.begin(); it != results.end(); ++it) {
    std::cout << (*it).name << std::endl;
    std::cout << "  mean     " << std::setw(20) << std::right
              << (*it).mean << std::endl;
    std::cout << "  variance " << std::setw(20) << std::right
              << (*it).variance << std::endl;
    std::cout << "  min      " << std::setw(20) << std::right
              << (*it).min << std::endl;
    std::cout << "  max      " << std::setw(20) << std::right
              << (*it).max << std::endl;
    std::cout << "  cyc/op   " << std::setw(20) << std::right
              << std::setprecision(1) << (*it).cycles_per_op << std::endl;
    std::cout << "  ns/op    " << std::setw(20) << std::right
              << std::setprecision(1) << (*it).nsecs_per_op << std::endl;
    std::cout << std::endl;
  }
  std::cout.imbue(std::locale());
  std::cout << std::endl;
#endif

  if (!PRINT_GO_BENCHSTAT_FORMAT) {
    std::cout << "Benchmark                                   cyc/op     ns/op"
              << std::endl;
    std::cout << "____________________________________________________________"
              << std::endl;
    for (auto it = results.begin(); it != results.end(); ++it) {
        std::cout << std::setw(40) << std::left  << (*it).name
                  << std::setprecision(1)
                  << std::setw(10) << std::right << (*it).cycles_per_op
                  << std::setw(10) << std::right << (*it).nsecs_per_op
                  << std::endl;
    }
  }

  std::cout << std::endl;
  auto endClock = std::chrono::system_clock::now();
  std::chrono::duration<double> runTime = endClock - startClock;
  std::cout << "Total runtime is: " << runTime.count() << " secs" << std::endl;

  return 0;
}
