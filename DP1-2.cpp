#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <cstdlib>
#include <climits>

using namespace std;

typedef enum { NOT_PAIR, PAIR_L, PAIR_R, } rnapair;

class rna{
  string header;  string sequence;  int length;
  rnapair *structure;  int *trace_back;  int *max_base_pair;
  int position(int i, int j){
    int pos = (length * i) - ((i - 1) * i / 2) + (j - i);
    return pos;
  }
public:
  void init(int i){ length = i;
    max_base_pair = new int [i * (i + 1) / 2];
    trace_back    = new int [i * (i + 1) / 2];
    structure     = new rnapair [i];
  }
  void destroy(){
    delete [] max_base_pair; delete [] trace_back;
    delete [] structure;
  }
  int get_mbp(int i, int j){
    return i > j ? 0 : max_base_pair[position(i, j)]; 
  }
  void set_mbp(int i, int j, int mbp){
    if(i <= j) max_base_pair[position(i, j)] = mbp; return ;
  }
  int get_trbk(int i, int j){
    return i > j ? 0 : trace_back[position(i, j)];
  }
  void set_trbk(int i, int j, int val){
    if(i <= j) trace_back[position(i, j)] = val; return ;
  }
  void set_strucure(int i, rnapair p){ structure[i] = p; }
  void read_from_file(char *);
  void secondary_structure_traceback(int, int);
  void secondary_structure_prediction();
  void show_results(int);
  char who_to_pair(char);
  void dump();
};

void rna::show_results(int width = INT_MAX){
  /* show the results of the 2D structure prediction algorithm */
  cout << header << endl;
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
  /* return an alphabet that can base pair with arguement c */
  char pair;
  switch(toupper(c)){
    case 'A': pair = 'U'; break;
    case 'C': pair = 'G'; break;
    case 'G': pair = 'C'; break;
    case 'U': pair = 'A'; break;
    default:  pair = ' '; break;
  }
  return pair;
}

void rna::dump(){
  cout << header << endl;
  cout << "length = " << length << endl;
  cout << "   ";
  cout << sequence << endl;
#if 0 /* sequenceが長く無い場合は最大pair数の表を表示してもよさそう */
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

void rna::secondary_structure_traceback(int i, int j){
  /* compute the structure of the subsequence S[i..j] using
   * traceback pointer */
  if( j - i < 2 ){ return; }
  else{
    int k = get_trbk(i, j);
    if(k == -1){
      set_strucure(j, NOT_PAIR);
      secondary_structure_traceback(i, j - 1);
    }else{
      set_strucure(j, PAIR_R); set_strucure(k, PAIR_L); 
      if(i < k + 1){ secondary_structure_traceback(i, k - 1); }
      if(k < j + 1){ secondary_structure_traceback(k + 1, j - 1); }
    }
    return;
  }
}

void rna::secondary_structure_prediction(){
  /* compute a secondary structure of the imput sequence
   * that maximizes base-pair count using Dynamic Programing */

  for(int i = 0; i < length; i++){ /* 初期化 (l = 1 に相当)*/
    set_mbp(i, i, 0); set_strucure(i, NOT_PAIR);
  }
  for(int l = 2; l <= length; l++){ /* 調べる部分配列の長さ */
    for(int i = 0; i < length - l; i++){ /* 部分配列の左端点 */
      int j = i + l, argmaxk = i, max = 0;
      for(int k = i; k <= j - 2; k++){
	/* case 1: jがkとpairを形成する場合を調べる */
	if(toupper(sequence[k]) 
	   == toupper(who_to_pair(sequence[j]))){
	  int pairwithk
	    = get_mbp(i, k - 1) + get_mbp(k + 1, j - 1) + 1;
	  if(pairwithk > max){ max = pairwithk; argmaxk = k; }
	}
      }
      if( max > get_mbp(i, j - 1) ){ 
	/* case 1: jがargmaxkとpairを形成する場合 */
	set_mbp(i, j, max); set_trbk(i, j, argmaxk);	
      }else{ /* case 2: jはpairを形成しない場合 */
	set_mbp(i, j, get_mbp(i, j - 1)); set_trbk(i, j, -1);
      }
    }
  }
  secondary_structure_traceback(0, length - 1);
  return;
}

int main(int argc, char *argv[]){
  if(argc < 2){
    cerr << "usage: $" << argv[0] << " <sequence file>" << endl;
    exit(1);
  }else{
    char *sequence_file = argv[1];
    rna data;
    data.read_from_file(sequence_file);

    data.secondary_structure_prediction();
    /* data.dump(); */
    data.show_results(60);

    data.destroy();
  }
  return 0;
}
