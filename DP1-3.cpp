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
  matrix<T> max_plus(matrix<T> m2){
    if(this->size(1) != m2.size(0)){
      cerr << "undefined operation for matrix" << endl; exit(1);
    }else{
      matrix<T> results(this->size(0), m2.size(1));
      for(int i = 0; i < this->size(0); i++){
	for(int j = 0; j < m2.size(1); j++){
	  T max = this->get(i, 0) + m2.get(0, j);
	  for(int s = 1; s < this->size(1); s++){
	    if(this->get(i, s) + m2.get(s, j) > max){
	      max = this->get(i, s) + m2.get(s, j);
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
  converter<string> conv;
public:
  int s_size(){ return ltrans.size(); }
  int a_size(){ return conv.size(); }
  void read_from_file_params(char *);
  void init(int alph_size, string alphabet, int state_size){
    for(int i = 0; i < alph_size; i++){
      /* converter に alphabetを入れる */
      conv.ctoi(alphabet.substr(i, 1));
    }
    ltrans.init(state_size, state_size, logl(0));
    lemit.init(state_size, alph_size, logl(0));
  }    
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
  int block_size;
  vector<int> ary;
public:
  sequence(string h, string s){ header = h; str = s; }
  int length(){ return str.length(); }
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
      head.erase(); str.erase();
      head = buf;
    }else{ str += buf; }
  }
  if(str.length() > 0){ /* push the last sequence */
    sequence temp(head, str);
    data.push_back(temp);
  }      
  
  ifs.close();

  return data;
}

int viterbi_compression(char *params, char *data){
#if 1
  hmm model;
  model.read_from_file_params((char *)"input/params.txt");
  //  cout << model;
  vector<sequence> temp = read_from_file_seq((char *)"input/hmm-fr-1.fa");
  //  cout << temp;
  //  cout << temp.at(0).length()<< endl;
#endif
  
  return 0;
}
  

int main(int argc, char *agrv[]){
  viterbi_compression((char *)"input/params.txt", (char *)"input/hmm-fr-1.fa");
    
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
