#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <deque>
#include <stack>
#include <climits>

using namespace std;
class converter;
class edge;
class graph;

#define NO_EDGE -1

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
    int newnum = (this -> counter)++;
    this -> number[key] = newnum;
    this -> str.push_back(key);
  }
  return this -> number[key];
}

class edge{ /* edgeを表す構造体 */
  int from__; /* 有効グラフの始点 */
  int to__;   /* 有効グラフの終点 */
  int dist__; /* 有効グラフのdistance */
public:
  void set(int f, int t, int c) { /* edgeに値を入れる */
    from__ = f; to__ = t; dist__ = c; return; 
  }
  void dump(){ /* edgeの内容を表示する */
    cout << from__ << "=>" << to__ << "(" << dist__ << ")" << endl;
  }
  int from(){ return from__; }
  int to(){   return to__; }
  int dist(){ return dist__; }
};

edge *converter::str_to_edge(string str){
  /* string型に格納されたdagfileの1行をparseして，edge構造体をつくる */
  int i = str.find(','), j = str.find(';');
  int fromnum = this -> stoi(str.substr(0, i));
  int tonum = this -> stoi(str.substr(i + 1, j - i - 1));
  int distnum; sscanf(str.substr(j + 1).c_str(), "%d", &distnum);

  edge *newedge = new edge;
  newedge->set(fromnum, tonum, distnum);
  //newedge->dump();
  return newedge;
}

class graph{
  /* グラフを表す構造体 隣接行列(adjacency matrix) で実装した */
  int **adjmat; /* adjacency matrix */
  int vertex_size__; 
  int edge_size__;
  int *topol;    /* トポロジカルソートの結果 */
  int *dist_sum; /* ある点からの距離の和 */
  int *trbk;     /* 最短路のトレースバック */
public:
  void init(int v, int e){ 
    vertex_size__ = v; edge_size__ = e; 
    adjmat = new int * [v];
    topol = new int [v];
    dist_sum = new int [v];
    trbk = new int [v];
    for(int i = 0; i < v; i++){ adjmat[i] = new int [v]; 
      for(int j = 0; j < v; j++){ adjmat[i][j] = NO_EDGE; }
    }
  } 
  void add_edge(int from, int to, int dist){ adjmat[from][to] = dist; }
  void add_edge(edge *);
  int vertex_size(){ return vertex_size__; }
  int edge_size(){ return edge_size__; }
  int get_dist(int from, int to){ return adjmat[from][to]; }
  void read_from_file(char *, converter &);
  void shortest_path_dump_vertex(converter &, int );
  void shortest_path_dump_each(converter &, vector<bool> &, int, int);
  void shortest_path_dump(converter &, int);
  void shortest_path(int);
  void topological_sort();
  void topological_sort_rec(int, int &, vector<bool> &);
  void dump();
  void destroy(){
    for(int i = 0; i < vertex_size__; i++){ delete [] adjmat[i]; }
    delete [] adjmat;   delete [] topol;
    delete [] dist_sum; delete [] trbk;
    return;
  }
};

inline void graph::add_edge(edge *e){
  adjmat[e -> from()][e -> to()] = e -> dist();  return;
}

inline void graph::shortest_path_dump_vertex(converter &conv, int i){
  cout << conv.itos(i)
       << "(" << dist_sum[i] << ")" 
       << " ";
  return;
}

inline void graph::shortest_path_dump_each(converter &conv, vector<bool> &visited,
				    int start, int dest){
  /* start から destへ至る path を traceback して 表示する */
  stack<int> path;  int i = dest;  
  while(i != start){ path.push(i); visited.at(i) = true; i = trbk[i]; }
  path.push(i); visited.at(i) = true;

  while(!path.empty()){
    shortest_path_dump_vertex(conv, path.top());
    path.pop();
  }
  cout << endl;
  return;
}

void graph::shortest_path_dump(converter &conv, int start){
  /* startから到達可能な点に関して, minimum distanceとpathを表示する */
  vector<bool> visited(vertex_size()); 
  /* visited: すでに表示されたpathの部分列が何度も表示されるのを防ぐための変数 */
  for(int i = 0; i < vertex_size(); i++){ visited.at(i) = false; }
  for(int i = vertex_size() - 1; i >= start; i--){
    if(visited.at(topol[i]) == false){ shortest_path_dump_each(conv, visited, start, topol[i]); }
  }
  return;

}

void graph::shortest_path(int start){
  for(int i = 0; i < vertex_size(); i++){
    dist_sum[i] = INT_MAX; trbk[i] = -1;
  }
  dist_sum[start] = 0; trbk[start] = 0;

  int i; /* 開始ノードを探す */
  for(i = 0; topol[i] != start && i < vertex_size(); i++);
  if(i == vertex_size()) return;


  for(; i < vertex_size(); i++){
    for(int j = i + 1; j < vertex_size(); j++){
      if( get_dist(i, j) != NO_EDGE && 
	  dist_sum[i] + get_dist(i, j) < dist_sum[j] ){
	dist_sum[j] = dist_sum[i] + get_dist(i, j);
	trbk[j] = i;
      }
    }
  }
#if 0
  cout << "minimum distance:   ";  dump_ary(dist_sum, vertex_size());
  cout << "trace back pointer: ";  dump_ary(trbk, vertex_size());
#endif
  return;
}


void graph::topological_sort_rec(int i, int &count, vector<bool> &visited){
  for(int j = 0; j < vertex_size(); j++){
    if(get_dist(i, j) != NO_EDGE){
      if(visited.at(j) == false){
	topological_sort_rec(j, count, visited);
      }else{ /* warning: it's not a DAG */
	cerr << "There exists a cyclic path including a edge "
	     << "(" << i << "," << j << ")" << endl;
        cerr << "This graph is NOT a DAG; aborting" << endl;
        exit(1);
      }
    }
  }
  topol[--count] = i;
  return;
}
  

void graph::topological_sort(){
  /* topological sortを実行して, topol [] にindexを格納 */
  if(vertex_size() < 2){ return; } /* arleady sorted */

  vector<bool> visited(vertex_size());
  for(int i = 0; i < vertex_size(); i++){ visited[i] = false; }
  int topological_num = vertex_size();

  topological_sort_rec(0, topological_num, visited);

#if 0 /* topological sortの結果を表示したい時に1 */
  dump_ary(topol, vertex_size());
#endif
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
      if(adjmat[i][j] == NO_EDGE){ printf("   - "); }
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
    cerr << "cannot open dag file" << endl; exit(1);
  }
  string buf; 
  deque<edge *> input_data; /* dagfileの内容は一度queueに格納する */
  while(getline(dagfile_fs, buf)){
    input_data.push_back(conv.str_to_edge(buf)); /* edge構造体にして格納*/
  }
  dagfile_fs.close();
  /* ファイルを閉じる */

  this -> init(conv.size(), input_data.size()); /* graph構造体の初期化*/

  while(input_data.size() > 0){ 
    add_edge(input_data.front()); /* graph構造体にedgeの情報を入れる*/ 
    delete input_data.front();    /* edge のオブジェクトをdeleteする */
    input_data.pop_front();       /* dequeから取り除く */
  }
  return;
}



int main(int argc, char *argv[]){
  if(argc < 2){
    cerr << "usage: $" << argv[0] 
	 << " <dagfile> <startVertex>" << endl;
  }else{
    char *dagfilepath = argv[1];
    converter conv;

    graph *g = new graph;
    g->read_from_file(dagfilepath, conv);

    int start = conv.stoi(argv[2]); /* 開始点 */
    g->topological_sort();    /* DAGの一列化 */
    g->shortest_path(start);
    g->shortest_path_dump(conv, start);

#if 0
    g->dump();
#endif    
    g->destroy();
    return 0;
  }
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
