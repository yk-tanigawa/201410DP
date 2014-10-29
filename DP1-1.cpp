#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <climits>

using namespace std;
class converter;
class node;
class edge;
class graph;

/* debug用のdump関数を用意 */
template <class T>
void dump_vec(vector<T> vec){
  for(int i = 0; i < vec.size(); i++){ cout << vec.at(i) << " "; }
  cout << endl; return;
}

template <class T>
void dump_ary(T *ary, int num){
  for(int i = 0; i < num; i++){ cout << ary[i] << " "; }
  cout << endl; return;
}

istream &getline_wocomment(char, istream &, string &);
istream &getline_wocomment(char c, istream &is, string &str){
  /*  read from stream without comments, * 
   *  'c' will be treated as a delimiter */
  getline(is, str);  int comment_start;
  while((comment_start= str.find(c)) == 0){getline(is, str);}
  if(comment_start > 0){ str.erase(comment_start); }
  return is;
}


/* class converter :
 * node name と int の数値を相互に変換する関数を提供する。 */
class converter{
  /* vertexを表す string型の名前 と int型の数値を相互に変換する */
  int counter; /* 今持っているvertexの数 */
  deque<string> str; /* int型のindexでstring型を返す */
  map<string, int> number; /* string型をkeyとしてintを返す */
public:
  converter(){ counter = 0; }
  int stoi(string);
  string itos(int i){ return str.at(i); }
  int size(){ return counter; }
  void debug();
  edge *str_to_edge(string); /* dagfileの1行からedge構造体を作る */
};

inline int converter::stoi(string key){
  if (this -> number.count(key) == 0) {
    /* keyをキーとする要素がmap中にない場合に追加する */
    int newnum = counter++;
    this -> number[key] = newnum;
    this -> str.push_back(key);
  }
  return this -> number[key];
}

/* edgeを表す */
class edge{
  int from;  int destination;  int weight;
public:
  edge(){ from = destination = weight = 0; }
  edge(int f, int d, int w){
    from = f; destination = d; weight = w; }
  void set(int f, int d, int w){
    from = f; destination = d; weight = w; }
  int f(){ return from;}
  int d(){ return destination; }
  int w(){ return weight; }
  friend ostream &operator<<(ostream &, edge *);
};

/* vertexを表すnode */
class node{
  vector<edge *> from_vector, edge_vector;
public:
  int from_size(){ return from_vector.size(); }
  int edge_size(){ return edge_vector.size(); }
  vector<edge *> from_v(){ return from_vector; }
  vector<edge *> edge_v(){ return edge_vector; }
  void add_edge(edge *e){ edge_vector.push_back(e); }
  void add_from(edge *e){ from_vector.push_back(e); }
};

class graph{
  vector<node> node__;
  vector<edge> edge__;
  converter conv;
  vector<int> topol;  /* topological sort した結果を格納 */
  vector<int> src;    /* source node を探索した結果を格納 */
  int  n_size(){ return node__.size(); }
  int  e_size(){ return edge__.size(); }
  void init(int node_num){ node__.resize(node_num); }  
  void add_edge(edge *);
  void topological_sort(int, vector<bool> &, int &, vector<int> &);
  void shortest_path_show(int, vector<int> &, vector<int> &);
  void shortest_path(int);
public:
  void read_from_file(char *);
  void dump();
  void topological_sort_forest();
  void shortest_path();
};

void graph::dump(){ 
  cout << "number of node : " << n_size() << endl;
  cout << "number of edge : " << e_size() << endl;
  for(int i = 0; i < n_size(); i++){
    cout << i << ", " << conv.itos(i) << " :";
    dump_vec(node__.at(i).edge_v());
    //dump_vec(node__.at(i).from_v());
  }
  return;
}

edge *converter::str_to_edge(string str){
  /* string型に格納されたdagfileの1行をparseして構造体をつくる */
  int i = str.find(','), j = str.find(';');
  int from_num = this -> stoi(str.substr(0, i));
  int to_num   = this -> stoi(str.substr(i + 1, j - i - 1));
  int weight; sscanf(str.substr(j + 1).c_str(), "%d", &weight);
  edge *e;  e  = new edge;
  e->set(from_num, to_num, weight);
  //cout << *e;
  return e;
}

ostream &operator<<(ostream &stream, edge *e){
  stream << e->f() << "=>" << e->d() << "(" << e->w() << ")" << " ";
  return stream;
}

void graph::add_edge(edge *e){
  /* edge の始点ノードと，終点ノードのそれぞれにポインタをセット */
  node__.at(e->f()).add_edge(e);
  node__.at(e->d()).add_from(e);
  return;
}

void graph::read_from_file(char *dagfile){
  ifstream dagfile_fs(dagfile);
  if ( dagfile_fs.fail() ){
    cerr << "cannot open dag file" << endl; exit(1);
  }

  /* ファイルに書かれたedgeを読み込む */
  string buf;
  while(getline_wocomment('#', dagfile_fs, buf)){ 
    if(buf.length() > 0 ) { edge__.push_back(*conv.str_to_edge(buf)); }
  }
  dagfile_fs.close(); /* file close */
  init(conv.size()); /* 初期化 */

  /* 各nodeにedgeへのポインタを張る */
  for(int i = 0; i < edge__.size(); i++){ add_edge(& edge__.at(i)); }

  return;
}

void graph::topological_sort(int k, vector<bool> &visited, int &id, vector<int> &topol_order){
  visited.at(k) = true;
  for(int i = 0; i < node__.at(k).edge_v().size(); i++){
    int d = node__.at(k).edge_v().at(i)->d();
    if(!visited.at(d)){
      topological_sort(d, visited, id, topol_order); 
    }else if(topol_order.at(k) <= topol_order.at(d)){    
      cerr << "Oops! there is a circuit." << endl
	   << conv.itos(k) << "=>" << conv.itos(d) << endl
	   << "This means that input file is NOT a DAG." << endl
	   << "Fatal error. Stop." << endl;
      delete this;
      exit(1);
    }
  }
  /* 再帰呼び出しが終わった順でtopol[]に格納 */
  topol.at(n_size() - (++id)) = k;
  topol_order.at(k) = id;
  return;
}

void graph::topological_sort_forest(){
  /* DAGに対してtopological sortを実行 */
  topol.resize(n_size());  int id = 0;
  vector<bool> visited(n_size(), false);
  vector<int> topol_order(n_size(), INT_MAX);
 /* 頂点を何番目にvisitしたか*/

  for(int k = 0; k < n_size(); k++){
    if(!visited.at(k)){
      topological_sort(k, visited, id, topol_order); 
      src.push_back(k); /* k はsource */
    }
  }
  // cout << "topol  : ";  dump_vec(topol); dump_vec(topol_order);
  // cout << "source : ";  dump_vec(src);
  return ;
}

void graph::shortest_path_show(int s, vector<int> &cost, vector<int> &tracebk){
  vector<bool> visited(n_size(), false);
  for(int i = n_size() - 1; i > s; i--){
    /* topol[] の後ろから順に，tracebackして結果を表示する */
    int j = topol.at(i);
    if((!visited.at(j)) && (cost.at(j) != INT_MAX)){
      /* tracebackしていない かつ jへのpathが存在するとき */
      stack<int> route; /* traceback してstackに積む */
      route.push(j); visited.at(j) = true;
      while(tracebk.at(j) != j){
	j = tracebk.at(j); route.push(j);
	visited.at(j) = true;
      }
      while(route.size() > 0){
	/* 結果を表示する */
	int i = route.top();
	cout << conv.itos(i) << "(" << cost.at(i) << ") ";
	route.pop();
      }
      cout << endl;
    }
  }
  return;
}

void graph::shortest_path(int k){
  /* source k から全点に対するshortest pathを求める */
  vector<int> cost(n_size(), INT_MAX); /* shortest path の weight */
  vector<int> tracebk(n_size(), -1);   /* trace back pointer */
  cost.at(k) = 0; tracebk.at(k) = k;
  int start = 0; /* k を topological order上でどこにあるか探す */
  while(topol.at(start) != k){ start++; }
  for(int i = start + 1; i < n_size(); i++){
    /* DP の実行 */
    int vertex = topol.at(i); int min = INT_MAX, argmin = i;
    for(int f = 0; f < node__.at(vertex).from_v().size(); f++){
      /* 自分 = vertex への枝をもつ全てのノードについて */
      edge *from = node__.at(topol.at(i)).from_v().at(f);
      if(cost.at(from->f()) != INT_MAX && /* 始点よりは前に戻らない */
	 (cost.at(from->f()) + from->w()) < min){
	/* min edges (u,v) (SP(u) + weight of (u,v) ) */
	min = cost.at(from->f()) + from->w();  argmin = from->f();
      }
    }
    cost.at(vertex) = min;  tracebk.at(vertex) = argmin;
  }
  // cout << start << endl;  dump_vec(cost);  dump_vec(tracebk);
  shortest_path_show(start, cost, tracebk);
  return;
}

void graph::shortest_path(){
  /*  vector<int> src の各点に対して shortest pathを求める */
  for(int i = 0; i < src.size(); i++){ shortest_path(src.at(i)); }
  return;
}

void converter::debug(){
  /* converter class の 動作確認 */
  converter conv;
  cout << conv.stoi("a") << endl;
  cout << conv.stoi("b") << endl;
  cout << conv.stoi("a") << endl;
  cout << conv.stoi("b") << endl;
  cout << conv.stoi("c") << endl;
  cout << conv.stoi("a") << endl;
  cout << conv.itos(0) << endl;
  cout << conv.itos(1) << endl;
  cout << conv.itos(0) << endl;
  cout << conv.itos(1) << endl;
  cout << conv.itos(2) << endl;
  cout << conv.itos(0) << endl;
  return;
}

int main(int argc, char *argv[]){
  if(argc < 2){
    cerr << "usage: $" << argv[0] << " <dagfile>" << endl;
    exit(1);
  }else{    
    graph *g = new graph;
    char *dagfilepath = argv[1];
    g->read_from_file(dagfilepath);
    //g->dump();
    g->topological_sort_forest();
    g->shortest_path();
    delete g;
    return 0;
  }
}
