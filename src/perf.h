// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef __SUPRANATIONAL_PERF_H__
#define __SUPRANATIONAL_PERF_H__

#include <cstdint>
#include <string>

class Perf {
  public:
    Perf(uint32_t results_length, uint32_t iterations);
    ~Perf();

    uint64_t calc_mean();
    uint64_t calc_mean(uint32_t length);
    uint64_t calc_mean(uint64_t *values, uint32_t length);

    uint64_t calc_variance();
    uint64_t calc_variance(uint32_t length);
    uint64_t calc_variance(uint64_t *values, uint32_t length);

    uint64_t get_cycles_per_sec();

    uint64_t get_min_result();
    uint64_t get_min_result(uint32_t length);
    uint64_t get_max_result();
    uint64_t get_max_result(uint32_t length);

    double   get_cycles_per_op();
    double   get_cycles_per_op(uint32_t length, uint32_t iters);
    double   get_nsecs_per_op();
    double   get_nsecs_per_op(uint32_t length, uint32_t iters);

    void     get_and_print_stats(uint64_t  cycles_per_sec,
                                 uint64_t* cycles_per_op,
                                 uint64_t* nsecs_per_op);

    void     print_results();

    void     print_go_benchstat_format(std::string name, uint32_t length,
                                       uint32_t iters);

    inline void start_collection() {
      this->starting_count = get_tsc();
    }

    inline void end_collection(uint32_t iter) {
      this->ending_count = get_tsc();
      this->results[iter] = this->ending_count - this->starting_count;
    }

    // Doesn't really check if Turbo is enabled
    inline bool check_turbo() {
      uint32_t a, b, c, d;
      a = 0x6;

      __asm__ volatile("cpuid;"
                  : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
                  : "a" (a)
                  : 
                 );
      return ((a & 0x01) > 0);
    }

  private:
    inline uint64_t get_tsc() {
      uint64_t count;
    
      // Read Time-Stamp Counter, Opcode - 0x0F 0x31, EDX:EAX <- TSC
      //__asm__ volatile(".byte 15; .byte 49; 
      __asm__ volatile("lfence;             \
                        .byte 15; .byte 49; \
                        shlq  $32,  %%rdx;  \
                        orq  %%rdx, %%rax;  \
                        lfence;"
                       : "=a" (count)
                       :
                       : "%rdx"
                      );
      return count;
    }


    uint64_t  cycles_per_sec;
    uint64_t  starting_count;
    uint64_t  ending_count;
    uint32_t  results_length;
    uint64_t* results;
    uint32_t  iterations;
};
#endif /* __SUPRANATIONAL_PERF_H__ */
