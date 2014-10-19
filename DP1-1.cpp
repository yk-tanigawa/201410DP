#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <deque>
#include <climits>

using namespace std;
class converter;
class edge;
class graph;

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

int converter::stoi(string key){
  if (this -> number.count(key) == 0) {
    /* keyをキーとする要素がmap中にない場合に追加する */
    int newnum = (this -> counter)++;
    this -> number[key] = newnum;
    this -> str.push_back(key);
  }
  return this -> number[key];
}

class edge{ /* edgeを表す構造体 */
  int from__; /* 有効グラフの始点 */
  int to__;   /* 有効グラフの終点 */
  int cost__; /* 有効グラフのcost */
public:
  void set(int f, int t, int c) { /* edgeに値を入れる */
    from__ = f; to__ = t; cost__ = c; return; 
  }
  void dump(){ /* edgeの内容を表示する */
    cout << from__ << "=>" << to__ << "(" << cost__ << ")" << endl;
  }
  int from(){ return from__; }
  int to(){   return to__; }
  int cost(){ return cost__; }
};

edge *converter::str_to_edge(string str){
  /* string型に格納されたdagfileの1行をparseして，edge構造体をつくる */
  int i = str.find(','), j = str.find(';');
  int fromnum = this -> stoi(str.substr(0, i));
  int tonum = this -> stoi(str.substr(i + 1, j - i - 1));
  int costnum; sscanf(str.substr(j + 1).c_str(), "%d", &costnum);

  edge *newedge = new edge;
  newedge->set(fromnum, tonum, costnum);
  //newedge->dump();
  return newedge;
}

class graph{
  /* グラフを表す構造体 隣接行列(adjacency matrix) で実装した */
  int **adjmat; /* adjacency matrix */
  int vertex_size__; 
  int edge_size__;
public:
  void init(int v, int e){ 
    vertex_size__ = v; edge_size__ = e; 
    adjmat = new int * [v];
    for(int i = 0; i < v; i++){ adjmat[i] = new int [v]; 
      for(int j = 0; j < v; j++){ adjmat[i][j] = INT_MAX; }    
    }
  } 
  void add_edge(int from, int to, int cost){ adjmat[from][to] = cost; }
  void add_edge(edge *);
  int vertex_size(){ return vertex_size__; }
  int edge_size(){ return edge_size__; }
  void read_from_file(char *, converter &);
  void dijkstra(converter &, int);
  void dump();
};

inline void graph::add_edge(edge *e){
  adjmat[e -> from()][e -> to()] = e -> cost();
  return;
}

template <class T>
void dump_vec(vector<T> vec){
  for(int i = 0; i < vec.size(); i++){ cout << vec.at(i) << " "; }
  cout << endl; return;
}

void graph::dijkstra(converter &conv, int start){
  vector<int> cost(vertex_size());
  vector<bool> visited(vertex_size());
  int next = start;
  for(int i = 0; i < start ; i++){ visited[i] = false; }
  visited[start] = true;
  for(int i = start + 1; i < vertex_size() ; i++){ visited[i] = false; }

  for(int i = 0, min = INT_MAX; i < vertex_size(); i++){
    cost[i] = adjmat[start][i]; /* 直接移動のコストをセット */
    next = (cost[i] < min && visited[i] == false) ? i : next;
  }

  dump_vec(cost);
  dump_vec(visited);
  cout << next << endl;
  
  for(int t = 0; t < vertex_size(); t++){
    start = next;
  }

  return;
}


void graph::dump(){/* class graph の objectの内容を表示する */
  cout << "number of vertex : " << vertex_size() << endl;
  cout << "number of edge   : " << edge_size() << endl;
  printf("  |");
  for(int j = 0; j < vertex_size(); j++){ printf("%4d ", j); }
  printf("\n");
  for(int i = 0; i < vertex_size(); i++){
    printf("%d |", i);
    for(int j = 0; j < vertex_size(); j++){
      if(adjmat[i][j] == INT_MAX){ printf("   - "); }
      else{ printf("%4d ", adjmat[i][j]); }
    }
    printf("\n");
  }
  return;
}

void graph::read_from_file(char *dagfile, converter &conv){
  /* dagfileのファイル名を持つファイルの内容を読み取り，
   * graph構造体にセットする*/
  ifstream dagfile_fs(dagfile);
  if ( dagfile_fs.fail() ){
    cerr << "cannot open dag file" << endl;
    exit(1);
  }

  string buf;
  deque<edge *> input; /* dagfileの内容は一度queueに格納する */
  while(getline(dagfile_fs, buf)){
    input.push_back(conv.str_to_edge(buf)); /* edge構造体にして格納*/
  }
  dagfile_fs.close();

  this -> init(conv.size(), input.size()); /* graph構造体の初期化*/

  while(input.size() > 0){ /* graph構造体にedgeの情報を入れる*/
    add_edge(input.front()); 
    input.pop_front();
  }
  return;
}




int main(){
  converter conv;
  graph *g = new graph;
  g->read_from_file((char *)"dagfile", conv);
  g->dump();
  g->dijkstra(conv, 0);
  return 0;
}



void converter::debug(){
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
