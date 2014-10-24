#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cfloat>

using namespace std;

class job;
class hmm;
class sequence;
string str_delete_space(string);
istream &getline_wocomment(char, istream &, string &);
string str_delete_space(string);

class job {
  hmm *model; vector <sequence *> data;
  /* 入力したデータと計算結果を全て保存する構造体 */
public:
  void init(hmm *m, vector <sequence *> seq){
    model = m; data = seq;
  }
  void dump();
  void viterbi();
  void viterbi_dump();
  void viterbi_delete();
  friend job *read_from_input_file(char *, char *);
};

class sequence{
  /* FASTAファイルの各配列に対してつくられる構造体
   * 配列ごとにviterbi変数や前向き変数，後ろ向き変数などを持つ */
  string header;  int length;  int *ary;  int *vit;
  int **trbk;
  long double **f;  long double **b;
public:
  void dump();
  void viterbi(hmm *);
  void viterbi_dump();
  void viterbi_delete(hmm *);
  friend sequence *seq_init(string header, int *ary, int length);
};

class hmm{
  /* パラメータファイルで与えられるHMMモデルを入れる構造体 */
  long double ** trans;   long double ** emit;
  long double ** ltrans;  long double ** lemit;
  int a_size;  int s_size;  char *alph;
  int alph_to_digit(char c){
    /* hmm の中に含まれるアルファベットの文字を受け取って、
     * 何番目の文字であるかindexを返す*/
    for(int i = 0; i < a_size; i++){
      if(alph[i] == toupper(c)){ return i; }
    }
    /* HMMのアルファベットテーブルの中に見つからない */
    cerr << "data file conteins unknown alphabet " << c << endl;
    exit(EXIT_FAILURE);
  }
public:
  void dump();
  void init(int _a_size, int _s_size);
  void init();
  void hmm_delete();
  vector <sequence *> get_data(ifstream &data_fs);
  int *data_convert(const string str){ /* str のデータをint *に変換 */
    int *data = new int [str.length()];
    for(int i = 0; i < str.length(); i++){
      data[i] = this -> alph_to_digit(str[i]);
    }
    return data;
  }
  int get_s_size(){ return s_size; }
  int get_a_size(){ return a_size; }
  long double get_trans(int i, int j){ return trans[i][j]; }
  long double get_ltrans(int i, int j){ return ltrans[i][j]; }
  long double get_emit(int i, int j){ return emit[i][j]; }
  long double get_lemit(int i, int j){ return lemit[i][j]; }
  void set_trans(int, int, long double);
  void set_emit(int, int, long double);
  friend job *read_from_input_file(char *, char *);
};

vector<string> split(const string &str, char delim){
  /* strを delim でsplitして vector<string> として返す */
  istringstream iss(str);
  string tmp;
  vector<string> res;
  while(getline(iss, tmp, delim)){
    res.push_back(tmp);
  }
  return res;
}

istream &getline_wocomment(char c, istream &is, string &str){
  /*  read from stream without comments, * 
   *  'c' will be treated as a delimiter */
  getline(is, str);  int comment_start;
  while((comment_start= str.find(c)) == 0){getline(is, str);}
  if(comment_start > 0){ str.erase(comment_start); }
  return is;
}

template <class T> 
void show_matrix(T **matrix, string format, int n, int m){
  /* 行列の内容をprintfを使って表示する。
   * 各要素のformatも引数として与える。*/
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++){
      printf(format.c_str(), matrix[i][j]);
    }
    printf("\n");
  }
  return;
}

template <class T> 
T ** allocat_mat(T ** matrix, int n, int m){
  /* 2D 配列にメモリを割り当てる */
  matrix = new T * [n];
  for(int i = 0; i < n; i++){
    matrix [i] = new T [m];
  }
  return matrix;
}

template <class T> 
void delete_mat(T ** matrix, int n, int m){
  /* メモリをfreeする */
  for(int i = 0; i < n; i++){
    delete [] matrix [i];
  }
  delete [] matrix;
  return;
}

template <class T>
int find_max_index(T *ary, int len){
  /* 長さlenの配列の要素で，最大のものが格納されているindexを返す */
  T max = ary[0];
  int index = 0;
  for(int i = 1; i < len; i++){
    if(ary[i] > max){ max = ary[i]; index = i; }
  }
  return index;
}

string str_delete_space(string buf){ /*スペースを消す*/
  string newstr;
  for(int i = 0; i < buf.length(); i++){
    if(buf[i] != ' '){ newstr += buf[i]; }
  }
  return newstr;
}

sequence *seq_init(string header, int *ary, int length){
  /* 引数で与えられたパラメタをsequence 構造体にセットして返す*/
  sequence *seq = new sequence;
  seq -> length = length;  seq -> header = header;
  seq -> ary = new int [length];
  for(int i = 0; i < length; i++){(seq -> ary)[i] = ary[i];}
  return seq;
}

vector <sequence *> hmm::get_data(ifstream &data_fs){
  /* データファイルに含まれる配列をsequence構造体のリストとして返す */
  vector<sequence *> data;
  string head, data_str, buf;

  while(getline(data_fs, buf)){
    if(buf[0] == '>'){
      /* 新しい配列の情報が始まった */
      if(data_str.length() > 0){
	/* 一つ前の配列の情報があるとき，それをvectorに格納する */
	data.push_back(seq_init(head, 
				data_convert(data_str), 
				data_str.length()));
      }
      head.erase();  head = buf; /* 次のヘッダー */
      data_str.erase(); /* 配列情報を破棄 */
    }else{ /* 前の配列の続き */
      data_str += buf;
    }
  }
  if(data_str.length() > 0){
    /* 一つ前の配列の情報があるとき，それをvectorに格納する */
    data.push_back(seq_init(head, data_convert(data_str), 
			    data_str.length()));
  }
  return data;
}

job *read_from_input_file(char *param_file, char *data_file){
  /* ファイルを開いてデータを読み込み，job構造体に格納する */
  /* open file stream */
  ifstream param_fs(param_file);
  if (param_fs.fail()){
    cerr << "cannot open params file" << endl;
    exit(1);
  }
  ifstream data_fs(data_file);
  if (data_fs.fail()){
    cerr << "cannot open data file" << endl;
    exit(1);
  }

  /* create a hmm model from input file*/
  hmm *model = new hmm;
  string buf; char comment_ch = '%';

  getline_wocomment(comment_ch, param_fs, buf);
  sscanf(buf.c_str(), "%d", &(model -> a_size));
  
  model -> alph = new char [model -> a_size];
  getline_wocomment(comment_ch, param_fs, buf);
  for(int i = 0; i < buf.length(); i++){
    model -> alph[i] =  toupper(str_delete_space(buf) [i]);
  }

  getline_wocomment(comment_ch, param_fs, buf);
  sscanf(buf.c_str(), "%d", &(model -> s_size));

  model -> init();

  /* transittion probability */
  for(int i = 0; i < model -> get_s_size(); i++){
    getline_wocomment(comment_ch, param_fs, buf);
    vector<string> input_str = split(buf, ' ');
    long double input;
    for(int j = 0; j < model -> get_s_size(); j++){
      sscanf(input_str[j].c_str(), "%Lf", &input);
      model ->  trans[i][j] = input;
      model -> ltrans[i][j] = logl(input);
    }
  }

  /* emittion probability */
  for(int i = 1; i < model -> get_s_size(); i++){
    getline_wocomment(comment_ch, param_fs, buf);
    vector<string> input_str = split(buf, ' ');
    long double input;
    for(int j = 0; j < model -> a_size; j++){
      sscanf(input_str[j].c_str(), "%Lf", &input);
      model ->  emit[i][j] = input;
      model -> lemit[i][j] = logl(input);
    }
  }
  for(int j = 0; j < model -> a_size; j++){
    model ->  emit[0][j] = 0;
    model -> lemit[0][j] = logl(0);
  }
  job *jb = new job;
  jb -> init(model, model -> get_data(data_fs));
  return jb;
}

void job::dump(){ /*job全体を表示*/
  model -> dump(); //まずモデルを表示して，各配列も表示
  for(int i = 0; i < data.size(); i++){ (data.at(i))->dump(); }
}

void hmm::dump(){
      /* HMM構造体の内容を表示する */
  cout << "number of alphabet :" << a_size << endl;
  cout << "alphabet are       :" << alph << endl;
  cout << "number of states   :" << s_size << endl;
  cout << "transition probability matrix is as follows: "
       << endl;
  show_matrix(trans,  "%10Lf", s_size, s_size);
  cout << "log transition probability matrix is as follows: "
       << endl;
  show_matrix(ltrans, "%10Lf", s_size, s_size);
  cout << "emittion probabiliry matrix is as follows: " 
       << endl;
  show_matrix(emit,   "%10Lf", s_size, a_size);
  cout << "log emittion probabiliry matrix is as follows: " 
       << endl;
  show_matrix(lemit,  "%10Lf", s_size, a_size);
  return;
}

void sequence::dump(){
  /* FASTAにかかれていたヘッダーと配列長も表示 */
  cout << header << endl << "length : " << length << endl;
  for(int i = 0; i < length; i++){ cout << ary[i];}
  cout << endl;
}

void hmm::init(int _a_size, int _s_size){
  a_size = _a_size;  s_size = _s_size;
  return this -> init();
}

void hmm::init(){ /*hmm構造体のメモリ領域を確保*/
  trans  = allocat_mat(trans,  s_size, s_size);
  ltrans = allocat_mat(ltrans, s_size, s_size);
  emit   = allocat_mat(emit,   s_size, a_size);
  lemit  = allocat_mat(lemit,  s_size, a_size);
  for(int j = 0; j < a_size; j++){
    emit[0][j] = 0;  lemit[0][j] = logl(0);
  }
  return;
}

void hmm::hmm_delete(){
  delete_mat(trans,  s_size, s_size);
  delete_mat(ltrans, s_size, s_size);
  delete_mat(emit,   s_size, a_size);
  delete_mat(lemit,  s_size, a_size);
  return;
}  

void job::viterbi(){
  for(int i = 0; i < data.size(); i++){ (data.at(i))->viterbi(model); }
}

void job::viterbi_dump(){
  for(int i = 0; i < data.size(); i++){
    (data.at(i))->viterbi_dump(); 
  }
}

void job::viterbi_delete(){
  for(int i = 0; i < data.size(); i++){ 
    (data.at(i))->viterbi_delete(model); 
  }
}

void sequence::viterbi_dump(){
  cout << header << endl;
  for(int t = 0; t < length; t++){ cout << vit[t]; }
  cout << endl; return;
}

void sequence::viterbi_delete(hmm *model){
  delete_mat(trbk, length, model -> get_s_size());  return;
}

void sequence::viterbi(hmm *model){
  vit = new int [length];
  long double *lv1, *lv2;
  lv1 = new long double [length];
  lv2 = new long double [length];
  trbk = allocat_mat(trbk, length, model -> get_s_size());
  
  /* Viterbi変数の初期化 */
  lv1[0] = logl(1);
  for(int s = 1; s < model -> get_s_size(); s++){ lv1[s] = logl(0); }

  /* Viterbiアルゴリズムの繰り返しステップ */
  long double *lv_before, *lv; /* viterbi 変数は表2行を交互に使う*/
  for(int t = 0; t < length; t++){
    int c = ary[t];
    if(t % 2 == 0){ lv_before = lv1; lv = lv2; }
    else          { lv_before = lv2; lv = lv1; }
    for(int l = 0; l < model -> get_s_size(); l++){ /* 状態が k => l に遷移した */
      long double max = -1 * INFINITY; int max_index = -1;
      for(int k = 0; k < model -> get_s_size(); k++){
	long double ltrans_kl = model -> get_ltrans(k, l);
	long double temp = lv_before[k] + ltrans_kl;
	if(temp > max){ max = temp; max_index = k; }
      }
      lv[l] = model->get_lemit(l, c) + max; /* viterbi変数を計算 */
      trbk[t][l] = max_index; /* trace back ポインタをセット */
    }
  }
  /* Viterbiアルゴリズムのトレースバック */
  vit[length - 1] = find_max_index(lv, model -> get_s_size());
  for(int t = length - 1; t > 0; t--){
    vit[t - 1] = trbk[t][vit[t]];
  }
  delete [] lv1;  delete [] lv2;
}

inline void hmm::set_trans(int k, int l, long double Akl){
  trans[k][l] = Akl; ltrans[k][l] = logl(Akl); return;
}

inline void hmm::set_emit(int k, int c, long double Ekc){
  emit[k][c] = Ekc;  lemit[k][c] = logl(Ekc);  return;
}


int main(int argc, char *argv[]){
  if(argc < 2){
    cerr << "usage: $" << argv[0] 
	 << " <parameter file> <FASTA file>" << endl;
    return EXIT_FAILURE;
  }else{
    job *myjob = read_from_input_file(argv[1], argv[2]);


    myjob -> dump();

    myjob -> viterbi();  myjob -> viterbi_dump(); myjob -> viterbi_delete();
  }
}


