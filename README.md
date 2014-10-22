201410DP
========
A lab course on dynamic programming


[現状]

1) DAG の shortest pathを求めるプログラム
 DP1-1.cpp に実装した
 $ ./DP1-1 dagfile startVertex

2) the RNA secondary structure prediction algorithm
 DP1-2.cpp に実装した
 $ ./DP1-2 sequence.fa


[今後]
1) DAG の shortest path を求めるプログラムを変更する。
   source, sinkを自分で探し，
   それら全てのペアの間のshortest pathを計算するように変更。
