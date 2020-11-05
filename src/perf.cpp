// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "perf.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <unistd.h>

Perf::Perf(uint32_t results_length, uint32_t iterations) : 
  results_length(results_length),
  iterations(iterations) {

  this->results = new uint64_t[results_length];
  this->cycles_per_sec = 1; // Get's set after calling get_cycles_per_sec()
}

Perf::~Perf() {
  delete[] (this->results);
}

uint64_t Perf::calc_mean(uint64_t *values, uint32_t length) {
  uint64_t total = 0;

  for (uint32_t i = 0; i < length; i++) {
    total += values[i];
  }

  return total/length;
}

uint64_t Perf::calc_mean() {
  return calc_mean(this->results, this->results_length);
}

uint64_t Perf::calc_mean(uint32_t length) {
  return calc_mean(this->results, length);
}

uint64_t Perf::calc_variance(uint64_t *values, uint32_t length) {
  uint64_t mean = (uint64_t) calc_mean(values, length);
  uint64_t var_sum = 0;

  for (uint32_t i = 0; i < length; i++) {
    var_sum += pow(int64_t(values[i]-mean), 2);
  }

  return var_sum / mean;
}

uint64_t Perf::calc_variance() {
  return calc_variance(this->results, this->results_length);
}

uint64_t Perf::calc_variance(uint32_t length) {
  return calc_variance(this->results, length);
}

uint64_t Perf::get_cycles_per_sec() {
  uint64_t cycles;
  uint64_t iters        = 100000;
  uint64_t inner_cycles = 20; // number of adds in loop

  // TODO - need ARM version
  __asm__ volatile(".byte    15; .byte 49; \
                    lfence;                \
                    shlq    $32,  %%rdx;   \
                    orq   %%rdx,  %%rax;   \
                    mov   %%rax,  %%r9;    \
                   .L1$:                   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    addq  %%rdx,  %%r8;    \
                    addq   %%r8,  %%rdx;   \
                    dec   %%rcx;           \
                    jnz   .L1$;            \
                    lfence;                \
                    .byte    15; .byte 49; \
                    shlq    $32,  %%rdx;   \
                    orq   %%rdx,  %%rax;   \
                    subq   %%r9,  %%rax;"
                    : "=a" (cycles)
                    : "c" (iters)
                    : "%r8", "%r9", "%rdx"
                  );

  double cycle_per_inst = (double) cycles / (iters * inner_cycles);

  if ((cycle_per_inst > 1.01) || (cycle_per_inst < 0.99)) {
    std::cout << "ERROR - clock frequency is not stable" << std::endl;
    std::cout << "cycle_per_inst not +/-1.0 : " << cycle_per_inst << std::endl;
    return 0;
  }

  uint64_t starting_cycles = get_tsc();
  usleep(1000000);
  uint64_t ending_cycles   = get_tsc();

  this->cycles_per_sec  = ending_cycles - starting_cycles;

  return this->cycles_per_sec;
}

uint64_t Perf::get_min_result() {
  return get_min_result(this->results_length);
}

uint64_t Perf::get_min_result(uint32_t length) {
  uint64_t min_value = UINTMAX_MAX;

  for (uint32_t i = 0; i < length; i++) {
    if (this->results[i] < min_value) {
      min_value = this->results[i];
    }
  }

  return min_value;
}

uint64_t Perf::get_max_result() {
  return get_max_result(this->results_length);
}

uint64_t Perf::get_max_result(uint32_t length) {
  uint64_t max_value = 0;

  for (uint32_t i = 0; i < length; i++) {
    if (this->results[i] > max_value) {
      max_value = this->results[i];
    }
  }

  return max_value;
}

double Perf::get_cycles_per_op(uint32_t length, uint32_t iters) {
  return ((double)this->calc_mean(length) / iters);
}

double Perf::get_cycles_per_op() {
  return get_cycles_per_op(this->results_length, this->iterations);
}

double Perf::get_nsecs_per_op(uint32_t length, uint32_t iters) {
  return (this->get_cycles_per_op(length, iters) / 
          this->cycles_per_sec) * 1000000000;
}

double Perf::get_nsecs_per_op() {
  return get_nsecs_per_op(this->results_length, this->iterations);
}

void Perf::get_and_print_stats(uint64_t  cycles_per_sec,
                               uint64_t* cycles_per_op,
                               uint64_t* nsecs_per_op) {
  uint64_t  mean      = this->calc_mean();
  uint64_t  variance  = this->calc_variance();
  uint64_t  min       = this->get_min_result();
  uint64_t  max       = this->get_max_result();

  double    cycles_per_op_dbl = ((double)mean / this->iterations);
  double    nsecs_per_op_dbl  = (cycles_per_op_dbl / cycles_per_sec)*1000000000;

  std::cout << "cyc/op  " << cycles_per_op_dbl << std::endl;
  std::cout << "nsec/op " << nsecs_per_op_dbl << std::endl;
  std::cout << "mean " << std::dec << mean
            << " variance " << variance << std::endl;
  std::cout << "min " << min << " max " << max << std::endl;

  *cycles_per_op = static_cast<uint64_t>(ceil(cycles_per_op_dbl));
  *nsecs_per_op  = static_cast<uint64_t>(ceil(nsecs_per_op_dbl));
}

void Perf::print_results() {
  std::cout << std::endl;
  for (uint32_t i = 0; i < this->results_length; i++) {
    if (i > 0) {
      std::cout << ","; 
    }
    std::cout << this->results[i];
  }
  std::cout << std::endl;
}

void Perf::print_go_benchstat_format(std::string name, uint32_t length,
                                     uint32_t iters) {
  double cycles_per_op;
  double nsecs_per_op;

  for (uint32_t i = 0; i < length; i++) {
    cycles_per_op = ((double) this->results[i] / iters);
    nsecs_per_op  = (cycles_per_op / this->cycles_per_sec) * 1000000000;
    std::cout << "Benchmark" << std::setw(30) << std::left << name
              << std::setprecision(3)
              << std::setw(12) << std::right << iters
              << std::setw(16) << std::right << nsecs_per_op << " ns/op"
              << std::endl;
  }
}
