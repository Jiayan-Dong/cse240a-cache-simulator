#!/bin/bash

make -C ./src

mkdir -p tmp

mkdir -p ./tmp/alpha
mkdir -p ./tmp/mips

bunzip2 -kc ./traces/gcc.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --prefetch > ./tmp/alpha/gcc.txt

bunzip2 -kc ./traces/bzip2.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --prefetch > ./tmp/alpha/bzip2.txt

bunzip2 -kc ./traces/namd.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --prefetch> ./tmp/alpha/namd.txt

bunzip2 -kc ./traces/h264.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --prefetch > ./tmp/alpha/h264.txt

bunzip2 -kc ./traces/gcc.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --prefetch > ./tmp/mips/gcc.txt

bunzip2 -kc ./traces/bzip2.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --prefetch > ./tmp/mips/bzip2.txt

bunzip2 -kc ./traces/namd.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --prefetch > ./tmp/mips/namd.txt

bunzip2 -kc ./traces/h264.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --prefetch > ./tmp/mips/h264.txt