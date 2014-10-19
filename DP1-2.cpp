#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <deque>
#include <climits>

using namespace std;



typedef enum { NOT_PAIR, PAIR_L, PAIR_R, } rnapair;


class rna{
  string header;
  string sequence;
  rnapair *structure;
  int *max_base_pair;
  int length;
  int position(int i, int j){
    int pos = (length * i) - ((i - 1) * i / 2) + (j - i);
    return pos;
  }
public:
  void init(int i){
    max_base_pair = new int [i * (i + 1) / 2];
    structure = new rnapair [i];
    length = i;
  }
  int get_mbp(int i, int j){
    return i > j ? 0 : max_base_pair[position(i, j)]; 
  }
  void set_mbp(int i, int j, int mbp){
    if(i <= j) max_base_pair[position(i, j)] = mbp; return ;
  }
  void set_strucure(int i, rnapair p){ structure[i] = p; }
  void read_from_file(char *);
  void secondary_structure_prediction();
  void show_results(int);
  char who_to_pair(char);
  void dump();
};

void rna::show_results(int width = INT_MAX){
  int lines = (length + width - 1) / width;
  for(int l = 0; l < lines - 1; l++){    
    cout << sequence.substr(width * l, width) << endl;
    for(int i = 0; i < width; i++){
      switch(structure[width * l + i]){
        case NOT_PAIR: cout << '.'; break;
        case PAIR_L:   cout << '('; break;
        case PAIR_R:   cout << ')'; break;
        default:       cout << ' '; break;
      }
    }
  cout << endl;
  }
  cout << sequence.substr(width * (lines - 1)) << endl;
  for(int i = width * (lines - 1); i < length; i++){
      switch(structure[i]){
        case NOT_PAIR: cout << '.'; break;
        case PAIR_L:   cout << '('; break;
        case PAIR_R:   cout << ')'; break;
        default:       cout << ' '; break;
      }
  }
  cout << endl;
  return;
}

char rna::who_to_pair(char c){
  char pair;
  switch(c){
    case 'a':  case 'A': pair = 'u'; break;
    case 'c':  case 'C': pair = 'g'; break;
    case 'g':  case 'G': pair = 'c'; break;
    case 'u':  case 'U': pair = 'a'; break;
    default:             pair = ' '; break;
  }
  return pair;
}

void rna::dump(){
  cout << header << endl;
  cout << "length = " << length << endl;
  cout << "   ";
  cout << sequence << endl;
#if 1
  cout << "   ";
  for(int j = 0; j < length; j++){ cout << j%10; } cout << endl;
  for(int i = 0; i < length; i++){
    printf("%2d|", i);
    for(int j = 0; j < i; j++) { printf(" "); }
    for(int j = i; j < length; j++){ printf("%d", get_mbp(i, j) % 10); }
    printf("\n");
  }
#endif
  return;
}

void rna::read_from_file(char *filename){
  ifstream seqfile_fs(filename);
  if ( seqfile_fs.fail() ){
    cerr << "cannot open RNA sequence file" << endl;
    exit(1);
  }

  getline(seqfile_fs, header);
  string buf;
  while(getline(seqfile_fs, buf)){
    sequence += buf;
  }
  seqfile_fs.close();
  
  init(sequence.length());
  
  return;
}

void rna::secondary_structure_prediction(){
  for(int i = 0; i < length; i++){ /* 初期化 l = 1 に相当*/
    set_mbp(i, i, 0); set_strucure(i, NOT_PAIR);
  }
  for(int l = 2; l <= length; l++){ /* 調べる部分配列の長さ */
    for(int i = 0; i < length - l; i++){
      int j = i + l - 1, argmaxk = i, max = 0;
      for(int k = i; k < j - 2; k++){
	/* case 1: jがkとpairを形成する場合を調べる */
	if(sequence[k] == who_to_pair(sequence[j])){
	  int pairwithk = get_mbp(i, k - 1) + get_mbp(k + 1, j - 1) + 1;
	  if(pairwithk > max){ max = pairwithk; argmaxk = k; }
	  //printf("case1: pair(%d, %d)\n", k, j);
	}
      }
      if( max > get_mbp(i, j - 1) ){ /* case 1: jがkとpairを形成する場合 */
	set_mbp(i, j, max);
	set_strucure(argmaxk, PAIR_L); set_strucure(j, PAIR_R);
      }else{ /* case 2: jはkとpairを形成しない場合 */
	set_mbp(i, j, get_mbp(i, j - 1));
	set_strucure(j, NOT_PAIR);
      }
    }
    // cout << "length  = " << l << "finished" << endl;
  }
  return;
}

int main(){

  rna data;
  data.read_from_file((char *)"sample-RNA2.fa");
  //  data.dump();

  data.secondary_structure_prediction();
  data.dump();
  data.show_results(60);



  return 0;
}
