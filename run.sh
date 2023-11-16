#!/bin/bash

make -C ./src

bunzip2 -kc ./traces/mat_20M.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 > alpha_with_stride_mat_20M.txt

bunzip2 -kc ./traces/insertsort.bz2 | ./src/cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100 > alpha_with_stride_insertsort.txt

bunzip2 -kc ./traces/mat_20M.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 > mips_with_stride_mat_20M.txt

bunzip2 -kc ./traces/insertsort.bz2 | ./src/cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100 > mips_with_stride_insertsort.txt