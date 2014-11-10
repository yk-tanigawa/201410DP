#include <sys/time.h>
#include "DP1-3.hpp"
#define MAX_BLOCK_SIZE 6
#define REPEAT_NUM 100

using namespace std;

long unsigned int calc_time(struct timeval, struct timeval);
long unsigned int calc_time(struct timeval s, struct timeval g){
  time_t diff_sec = g.tv_sec - s.tv_sec;
  suseconds_t diff_usec = g.tv_usec - s.tv_usec;
  long unsigned int diff = diff_sec * 1000000 + diff_usec;
  return diff;
}
			    
int main(){
  char * params = (char *)"input/DP1-3/params.txt";
  vector<char *> file(4); /* file name */
  for(int i = 0; i < 4; i++){
    file.at(i) = new char [20];
    sprintf(file.at(i), "input/DP1-3/hmm-fr-%d.fa", i + 1);
  }

  struct timeval ts, tg;
  matrix<long unsigned int> time(MAX_BLOCK_SIZE, 4, 0);
  /* 測定した時間を入れる */
  matrix<long double> performance(MAX_BLOCK_SIZE - 1, 4, 0);
  /* どれくらい速くなったかを入れる */

  for(int l = 0; l < time.size(); l++){    
    for(int i = 0; i < 4; i++){
      for(int n = 0; n < REPEAT_NUM; n++){
	/* 実行時間を測定 */
	gettimeofday(&ts, NULL);
	viterbi_compression(params, file.at(i), l + 1);
	gettimeofday(&tg, NULL);
	time.set(l, i, time.get(l, i) + calc_time(ts, tg));
      }
    }
  }

  for(int l = 0; l < performance.size(); l++){    
    for(int i = 0; i < 4; i++){
      /* performanceを計算 */
      performance.set(l, i, 1.0 * time.get(0, i) / time.get(l + 1, i));
    }
  }

  for(int i = 0; i < 4; i++){
    delete [] file.at(i);
  }

#if 0
  cout << time;
  cout << endl;
#endif

  cout << performance;

  return 0;
}
