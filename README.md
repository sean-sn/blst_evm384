# blst_evm384

Assembly implementation of EVM384 precompiles using blst

Includes translation of assembly to Go assembly for current x86 processors

Assembly code performs generic 384-bit modular operations (add, sub, mul)

See [EVM384](https://notes.ethereum.org/@axic/evm384) post for more details on the proposal

## Status
**Warning - this library is a work in progress**

## C 

### Build
./build.sh

### Re-run benchmark
./bench_eip2537

Note the benchmark expects a stable frequency

### Stablize CPU Operation

In order to get consistent results run to run and to compare against other platforms, a true operation cycle count is collected.  This requires the CPU frequency to be stable during the run.  

Disable turbo and frequency scaling (requires root access)

```
for i in /sys/devices/system/cpu/cpu\*/cpufreq/scaling_governor
do
  echo "performance" | sudo tee /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor
done
```

Intel
```
echo "1" | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
```

AMD
```
echo "0" | sudo tee /sys/devices/system/cpu/cpufreq/boost
```

Another option is to disable hyper threading if results are inconsistent


## Go

go test -bench=.
