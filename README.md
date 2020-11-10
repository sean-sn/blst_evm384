# blst_evm384

Assembly implementation of EVM384 precompiles using blst

Includes translation of assembly to Go assembly for current x86 processors

Assembly code performs generic 384-bit constant time modular operations (add, sub, mul)

Includes a version of constant time C code for 64-bit operating systems

See [EVM384](https://notes.ethereum.org/@axic/evm384) post for more details on the proposal

## Status
**Warning - this library is a work in progress**

For EVM384 performance demonstration purposes only

## C 

### Build
./build.sh

### Test
Random testing comparing C code output with assembly

./test_evm384

### Re-run benchmark
./bench_evm384

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


## Performance

Example of results from 3970X AMD running at stable 3.7 GHz

Assembly 
| Function | cycles/op | ns/op at 3.7 GHz|
| :------- | :-------: | :-------------: |
| Add      |   18.6    |     5.0         |
| Sub      |   18.8    |     5.1         |
| Mul      |  119.7    |    32.3         |

Use the cycles/op to determine performance on your machine.  Divide the cycles/op by your frequency (in GHz) to get nsecs/op.

For example from above:

18.6 / 3.7 = 5.0 ns/op

Go Assembly 
| Function | cycles/op | ns/op at 3.7 GHz|
| :------- | :-------: | :-------------: |
| Add      |   18.8    |     5.1         |
| Mul      |  128.0    |    34.6         |

Note the functions are generic 384-bit, therefore performance should be field size independent for all curves 384-bit and under.
