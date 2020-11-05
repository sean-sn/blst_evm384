if [ ! -d blst_asm ]; then
  mkdir blst_asm
  cd blst_asm

  wget --no-check-certificate https://raw.githubusercontent.com/supranational/blst/master/src/asm/add_mod_384-armv8.pl
  wget --no-check-certificate https://raw.githubusercontent.com/supranational/blst/master/src/asm/add_mod_384-x86_64.pl
  wget --no-check-certificate https://raw.githubusercontent.com/supranational/blst/master/src/asm/mul_mont_384-armv8.pl
  wget --no-check-certificate https://raw.githubusercontent.com/supranational/blst/master/src/asm/mulq_mont_384-x86_64.pl
  wget --no-check-certificate https://raw.githubusercontent.com/supranational/blst/master/src/asm/mulx_mont_384-x86_64.pl
  wget --no-check-certificate https://raw.githubusercontent.com/supranational/blst/master/src/asm/arm-xlate.pl
  wget --no-check-certificate https://raw.githubusercontent.com/supranational/blst/master/src/asm/x86_64-xlate.pl

  mkdir win64
  mkdir elf
  mkdir coff
  mkdir mach-o

  PERL=${PERL:-perl}

  for pl in *-x86_64.pl; do
    s=`basename $pl .pl`.asm
    expr $s : '.*portable' > /dev/null || (set -x; ${PERL} $pl masm > win64/$s)
    s=`basename $pl .pl`.s
    (set -x; ${PERL} $pl elf > elf/$s)
    (set -x; ${PERL} $pl mingw64 > coff/$s)
    (set -x; ${PERL} $pl macosx > mach-o/$s)
  done

  for pl in *-armv8.pl; do
    s=`basename $pl .pl`.asm
    (set -x; ${PERL} $pl win64 > win64/$s)
    s=`basename $pl .pl`.S
    (set -x; ${PERL} $pl linux64 > elf/$s)
    (set -x; ${PERL} $pl coff64 > coff/$s)
    (set -x; ${PERL} $pl ios64 > mach-o/$s)
  done

  cd ..
fi

g++ -Iblst_asm -march=native -O3  src/perf.cpp src/bench_evm384.cpp src/assembly.S -o bench_eip2537

./bench_eip2537
