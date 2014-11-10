#=======================================================#
# 201410DP ---- A lab course on dynamic programming --- #
# 	   	      	     			    	#
# source codes are available on GitHub                  #
#   https://github.com/yk-tanigawa/201410DP 		#
#=======================================================#

0) contents
 +source code
  * DP1-1.cpp
  * DP1-2.cpp
  * DP1-3.hpp
  * DP1-3_bench.cpp
  * DP1-3_main.cpp
  * Makefile

1) Finding the shortest path on a DAG
 The program is written in DP1-1.cpp
 $ ./DP1-1 dagfile

2) the RNA secondary structure prediction algorithm
 The program is written in DP1-2.cpp
 $ ./DP1-2 sequence.fa

3) Viterbi (Compression version)
 The main part of the program is written in DP1-3.hpp
 $ ./DP1-3_main <params> <sequence> <block size>
 $ ./DP1-3_bench
