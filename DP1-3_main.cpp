#include "DP1-3.hpp"

int main(int argc, char *argv[]){
  if(argc < 4){
    cerr << "usage: $" << argv[0] 
	 << " <param file> <sequence file> <block size>" 
	 << endl;
    exit(1);
  }else{
    int block_size = atoi(argv[3]);
    cout << viterbi_compression(argv[1], argv[2], block_size);
    return 0;
  }
}
