#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

class hmm;
class sequence;
template <class T> class converter;
template <class T> class matrix;
class fourRussian;

template <class T>
matrix<T> mat_max_plus(matrix<T>, matrix<T>);
int fact(int base, int exp);

int fact(int base, int exp){ /* 階乗の計算 */
  if(exp < 1){ /* x^0 = 1 と約束する */
    return 1; 
  }else if(exp % 2 == 0){
    return fact(base, (exp/2)) * fact(base, (exp/2));
  }else{
    return base * fact(base, exp - 1);
  }
}

/* debug用のdump関数を用意 */
template <class T>
void dump(vector<T> vec){
  for(int i = 0; i < vec.size(); i++){ cout << vec.at(i) << " "; }
  cout << endl; return;
}
template <class T>
void dump(T *ary, int num){
  for(int i = 0; i < num; i++){ cout << ary[i] << " "; }
  cout << endl; return;
}

template <class T>
ostream &operator<<(ostream &stream, vector<T> vector){
  for(int i = 0; i < vector.size() - 1; i++){
    stream << vector.at(i) << ", "; 
  }
  stream << vector.back() << endl;
  return stream;
}

template <class T>
ostream &operator<<(ostream &stream, vector< vector<T> > matrix){
  for(int i = 0; i < matrix.size(); i++){ stream << matrix.at(i); }
  return stream;
}

istream &getline_wocomment(char c, istream &is, string &str){
  /*  read from stream without comments, * 
   *  'c' will be treated as a delimiter */
  getline(is, str);  int comment_start;
  while((comment_start= str.find(c)) == 0){getline(is, str);}
  if(comment_start > 0){ str.erase(comment_start); }
  return is;
}

string str_delete_space(string buf){ /* スペースを消す */
  string newstr;
  for(int i = 0; i < buf.length(); i++){
    if(buf[i] != ' '){ newstr += buf[i]; }
  }
  return newstr;
}

/* class converter :
 * class T と int の数値を相互に変換する関数を提供する。 */
template <class T>
class converter{
  /* class T型の要素とclass int型の数値を相互に変換する */
  int counter;        /* 今持っているclass T型の要素の数 */
  vector<T> str;      /* int型のindexでclass T型を返す */
  map<T, int> number; /* class T型をkeyとしてint型を返すmap */
public:
  converter(){ counter = 0; }
  int size(){ return counter; }
  T itoc(int i){ return str.at(i); }
  int ctoi(T key){
    if(number.count(key) == 0){
      /* map 中に keyをキーとする要素がない場合に追加 */
      int newnum = counter++;
      number[key] = newnum; str.push_back(key);
    }
    return number[key];
  }
  friend ostream &operator<<(ostream &stream, converter<T> conv){
    stream << "number of items: " << conv.counter << endl;
    for(int i = 0; i < conv.counter; i++){
      stream << i << "\t" << conv.str.at(i) << endl;
    }
    return stream;
  }
};

template <class T>
class matrix{ /* 行列 : 独自のmax_plusというoperator のために定義 */
  vector<vector <T> > mat;
public:
  matrix(){}
  void init(int x, int y, T value = 0){
    mat.resize(x); /* size(x, y)の行列にして, valueをセットする */
    for(int i = 0; i < x; i++){ mat.at(i).resize(y, value); }
  }
  matrix(int x, int y, T value = 0){ init(x, y, value); }
  int size(int component = 0){
    if(component == 0) { return mat.size(); }
    else if(component == 1){ return mat.at(0).size(); }
    else { return -1; }
  }
  void set(vector <vector <T> > matrix){ mat = matrix; }
  void set(int x, vector<T> v){ mat.at(x) = v; }
  void set(int x, int y, T val){ mat.at(x).at(y) = val; }
  vector<vector <T> > get(){ return mat; }
  vector<T> get(int x){ return mat.at(x); }
  T get(int x, int y){ return mat.at(x).at(y); }
  friend matrix<T> mat_max_plus(matrix<T> m1, matrix<T> m2){
    if(m1.size(1) != m2.size(0)){
      cerr << "undefined operation for matrix" << endl; exit(1);
    }else{
      matrix<T> results(m1.size(0), m2.size(1));
      for(int i = 0; i < m1.size(0); i++){
	for(int j = 0; j < m2.size(1); j++){
	  T max = m1.get(i, 0) + m2.get(0, j);
	  for(int s = 1; s < m1.size(1); s++){
	    if(m1.get(i, s) + m2.get(s, j) > max){
	      max = m1.get(i, s) + m2.get(s, j);
	    }
	  }
	  results.set(i, j, max);
	}
      }
      return results;
    } 
  }
  friend ostream &operator<<(ostream &stream, matrix<T> m){
    stream << m.mat; return stream;
  }
};

class hmm{
  vector<long double> v0;
  matrix<long double> ltrans, lemit;
  string alphabet;
  vector<matrix <long double> > m;
  converter<string> conv;
public:
  int s_size(){ return ltrans.size(); }
  int a_size(){ return alphabet.length(); }
  void read_from_file_params(char *);
  void init(int alph_size, string alph, int state_size){
    alphabet = alph;  m.resize(alph_size);
    transform(alphabet.begin(), alphabet.end(), alphabet.begin(), ::toupper);
    for(int i = 0; i < alph_size; i++){
      /* converter に alphabetを入れる */
      conv.ctoi(alphabet.substr(i, 1));
    }
    for(int i = 0; i < alph_size; i++){
      m.at(i).init(state_size, state_size, logl(0));
    }
    ltrans.init(state_size, state_size, logl(0));
    lemit.init(state_size, alph_size, logl(0));
    
  }    
  matrix<long double> mat(int i){ return m.at(i); }
  vector<matrix<long double> > mat(){ return m; }
  int ctoi(string key){return conv.ctoi(key);}
  string itoc(int i){ return conv.itoc(i); }
  friend ostream &operator<<(ostream &stream, hmm model){
    stream << model.conv;
    stream << "log transition probability" << endl;
    stream << model.ltrans;
    stream << "log emittion probability" << endl;
    stream << model.lemit;
    return stream;
  }
};

class sequence{
  string header; /* header information */
  string str;    /* string */
public:
  sequence(){}
  sequence(string h, string s){ header = h; str = s; }
  int length(){ return str.length(); }
  string substr(string::size_type index,
		string::size_type len = string::npos){
    return str.substr(index, len);
  };
  friend vector<sequence> read_from_file_seq(char *);
  friend ostream &operator<<(ostream &stream, sequence seq){
    stream << seq.header << endl;
    stream << seq.str << endl;
    return stream;
  }
};

vector<long double> split(const string &str, char delim){
  /* strを delim でsplitして vector<long double> として返す */
  istringstream iss(str);  string tmp_str;  vector<long double> res;
  while(getline(iss, tmp_str, delim)){
    long double value;
    sscanf(tmp_str.c_str(), "%Lf", &value);
    res.push_back(value);
  }
  return res;
}

vector<long double> read_vector(char comment_delim, char vec_delim, istream &is){
  string buf;
  getline_wocomment(comment_delim, is, buf);
  return split(buf, vec_delim);
}

template <class T>
vector <long double> logl(vector<T> vec){
  vector <long double> results(vec.size());
  for(int i = 0; i < vec.size(); i++){
    results.at(i) = logl(vec.at(i));
  }
  return results;
}

void hmm::read_from_file_params(char *file){
  ifstream ifs(file);
  if( ifs.fail() ){ 
    cerr << "cannot open param file" << endl;
    exit(1);
  }
  
  string buf;
  getline_wocomment('%', ifs, buf);
  int alph_size = stoi(buf);
  getline_wocomment('%', ifs, buf);
  string alph = str_delete_space(buf);
  getline_wocomment('%', ifs, buf);
  int state_size = stoi(buf) - 1;
  /* 初期状態 s_0 を取り除くために -1  */

  init(alph_size, alph, state_size);

  vector<long double> temp;
  /* initial distribution*/
  temp = read_vector('%', ' ', ifs); temp.erase(temp.begin());
  v0 = logl(temp);
  /* log transittion probability */
  for(int i = 0; i < state_size; i++){
    temp = read_vector('%', ' ', ifs); temp.erase(temp.begin());
    ltrans.set(i, logl(temp));
  }
  /* log emittion probability */
  for(int i = 0; i < state_size; i++){
    temp = read_vector('%', ' ', ifs);
    lemit.set(i, logl(temp));
  }

  /* m : Matrix for Viterbi Calculation */
  for(int c = 0; c < alph_size; c++){
    for(int i = 0; i < state_size; i++){
      for(int j = 0; j < state_size; j++){
	m.at(c).set(i, j, lemit.get(i, c) + ltrans.get(i, j));
      }
    }
  }
  ifs.close();
}

vector<sequence> read_from_file_seq(char *file){
  ifstream ifs(file);
  if( ifs.fail() ){ 
    cerr << "cannot open sequence file: " << file << endl;
    exit(1);
  }
  string buf, head, str;
  vector<sequence> data;

  while(getline(ifs, buf)){
    if(buf.at(0) == '>'){ /* new sequence */
      if(str.length() > 0){ /* push previous sequence*/
	sequence temp(head, str);
	data.push_back(temp);
      }      
      head.erase(); str.erase(); head = buf;
    }else{ str += buf; }
  }
  if(str.length() > 0){ /* push the last sequence */
    sequence temp(head, str);
    data.push_back(temp);
  }      
  
  ifs.close();

  return data;
}


class fourRussian{
  /* acceleration by compression の本体 */
  hmm model;
  sequence seq;
  vector<int> comp_seq; /* compressed sequence */
  int block_size;
  map<string, int> dictionary;
  vector<matrix <long double> > mw; /* M(W) */
  int find_number(string str){
    int num = 0;
    for(int i = 0; i < str.length(); i++){
      num *= model.a_size();
      num += model.ctoi(str.substr(i, 1));
    }
    return num;
  }
  int dict(string str){
    if(dictionary.count(str) == 0){
      /* map 中に strをキーとする要素がない場合に追加 */
      int number = find_number(str);
      dictionary[str] = number;
      return number;
    }else{
      return dictionary[str];
    }
  }
  vector<matrix <long double> > encoding_body(int);
public:
  fourRussian(hmm &m, sequence &s, int b){
    model = m; seq = s; block_size = b;
  }
  int b_size(){ return block_size; }
  void dict_selection(){
    /* all possible string of length l are good string */
  }
  void encoding();
  void parsing();
  void propagation();
  friend int viterbi_compression(char *, char *, int);
};

vector<matrix <long double> > fourRussian::encoding_body(int i){
  if(i < 2){ return model.mat(); }
  else{
    int a_size = model.a_size();
    vector<matrix <long double> > prev = encoding_body(i - 1);
    vector<matrix <long double> > results(prev.size() * a_size);
    for(int i = 0; i < a_size; i++){
      for(int j = 0; j < prev.size(); j++){
	results.at(i * a_size + j)
	  = mat_max_plus(model.mat(i), prev.at(j));
      }
    }    
    return results;
  }
}

void fourRussian::encoding(){
  /*   たとえばblock_size = 4のとき次のようなwに対するM(W)が計算される
   *   0  aaaa
   *   1  aaac
   *   2  aaag
   *   3  aaat
   *   ... 
   * 255  tttt
   */
  mw = encoding_body(block_size);
}


void fourRussian::parsing(){
  comp_seq.resize(seq.length() / b_size());
  for(int i = 0; i < comp_seq.size(); i++){
    comp_seq.at(i) = dict(seq.substr(i * b_size(), b_size()));
  }
}

void fourRussian::propagation(){
  
}

int viterbi_compression(char *params, char *data, int block_size){
  hmm model;
  model.read_from_file_params((char *)"input/params.txt");
  cout << model;
  vector<sequence> seqs 
    = read_from_file_seq((char *)"input/hmm-fr-1.fa");
  cout << seqs;
  fourRussian compression(model, seqs.at(0), block_size);
  compression.encoding();
  compression.parsing();
  compression.propagation();
  //cout << compression.comp_seq;
  
  return 0;
}
  

int main(int argc, char *agrv[]){
  viterbi_compression((char *)"input/params.txt", (char *)"input/hmm-fr-1.fa", 1);
    
#if 0
  matrix<int> m1(2,2), m2(2,2);
  m1.set(0,0,1);
  m1.set(0,1,2);
  m1.set(1,0,3);
  m1.set(1,1,4);
  m2.set(0,0,3);
  m2.set(0,1,4);
  m2.set(1,0,5);
  m2.set(1,1,6);
  cout << m1;
  cout << m2;
  cout << m1.max_plus(m2);
#endif
  return 0;
}
