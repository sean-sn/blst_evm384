// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef __SUPRANATIONAL_BENCH_H__
#define __SUPRANATIONAL_BENCH_H__

#include "perf.h"

#define OUTER_ITERS_FAST 10
#define INNER_ITERS_FAST 1000000

struct BenchResult {
  bool        placeholder;
  uint64_t    mean;
  uint64_t    variance;
  uint64_t    min;
  uint64_t    max;
  double      cycles_per_op;
  double      nsecs_per_op;
  std::string name;
};

typedef void (*bench_func_ptr_t)(Perf*,
                                 std::uniform_int_distribution<uint64_t>&,
                                 std::uniform_int_distribution<uint64_t>&,
                                 std::vector<BenchResult>&);

#define WARM_UP_AND_BENCH(funcName, dst, outerIters, innerIters, func, ...)\
  BenchResult result;\
  result.placeholder = false; \
  result.name  = #funcName;\
  result.name += #dst;\
  for (int i = 0; i < outerIters; i++) {\
    func(__VA_ARGS__);\
  }\
\
  for (int i = 0; i < outerIters; i++) {\
    perf->start_collection();\
    for (int j = 0; j < innerIters; j++) {\
      func(__VA_ARGS__);\
    }\
    perf->end_collection(i);\
  }\
\
  result.mean          = perf->calc_mean(outerIters);\
  result.variance      = perf->calc_variance(outerIters);\
  result.min           = perf->get_min_result(outerIters);\
  result.max           = perf->get_max_result(outerIters);\
  result.cycles_per_op = perf->get_cycles_per_op(outerIters, innerIters);\
  result.nsecs_per_op  = perf->get_nsecs_per_op(outerIters, innerIters);\
  results.push_back(result);\
  if (PRINT_GO_BENCHSTAT_FORMAT) {\
    perf->print_go_benchstat_format(result.name, outerIters, innerIters);\
  }

#define BENCH_BOTH(outerIters, innerIters, funcName, func, ...)\
void Bench##funcName##Same(Perf* perf,\
                           std::uniform_int_distribution<uint64_t>& rng,\
                           std::uniform_int_distribution<uint64_t>& rng_upper,\
                           std::vector<BenchResult>& results) {\
  uint64_t  x[6];\
  uint64_t  y[6];\
  uint64_t* dest = x;\
\
  std::mt19937_64 gen(1);\
  for (int i = 0; i < 5; ++i) {\
    x[i] = rng(gen);\
    y[i] = rng(gen);\
  }\
  x[5] = rng_upper(gen);\
  y[5] = rng_upper(gen);\
\
  WARM_UP_AND_BENCH(funcName, Same, outerIters, innerIters, func, __VA_ARGS__)\
}\
void Bench##funcName##Diff(Perf* perf,\
                           std::uniform_int_distribution<uint64_t>& rng,\
                           std::uniform_int_distribution<uint64_t>& rng_upper,\
                           std::vector<BenchResult>& results) {\
  uint64_t  x[6];\
  uint64_t  y[6];\
  uint64_t  dest[6];\
\
  std::mt19937_64 gen(1);\
  for (int i = 0; i < 5; ++i) {\
    x[i] = rng(gen);\
    y[i] = rng(gen);\
  }\
  x[5] = rng_upper(gen);\
  y[5] = rng_upper(gen);\
\
  WARM_UP_AND_BENCH(funcName, Diff, outerIters, innerIters, func, __VA_ARGS__)\
}

#endif /* __SUPRANATIONAL_BENCH_H__ */
