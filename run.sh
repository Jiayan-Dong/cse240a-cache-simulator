#!/bin/bash

make -C ./src

mkdir -p tmp

mkdir -p ./tmp/alpha
mkdir -p ./tmp/mips

bunzip2 -kc ./traces/mat_20M.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --inclusive > ./tmp/alpha/mat_20M.txt

bunzip2 -kc ./traces/insertsort.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 --inclusive > ./tmp/alpha/insertsort.txt

bunzip2 -kc ./traces/mat_20M.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --inclusive > ./tmp/mips/mat_20M.txt

bunzip2 -kc ./traces/insertsort.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 --inclusive > ./tmp/mips/insertsort.txt