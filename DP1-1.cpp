#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <deque>
#include <stack>
#include <list>
#include <climits>

using namespace std;
class converter;
class input_line;
class node;
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
  input_line *str_to_edge(string); /* dagfileの1行からinput_line構造体を作る */
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

class input_line{
  int origin;  int target;  int weight;
public:
  void set(int o, int t, int w){origin = o; target = t; weight = w; }
  int from(){ return origin; }
  int to(){ return target; }
  int w(){ return weight; }
  friend ostream &operator<<(ostream &, input_line &);
};

/* edgeを表す */
class edge{
  int destination;
  int weight;
public:
  edge(){ destination = weight = 0; }
  edge(int d, int w){ destination = d; weight = w; }
  void set(int d, int w){ destination = d; weight = w; };
  int d(){ return destination; }
  int w(){ return weight; }
  friend ostream &operator<<(ostream &, edge &);
};

/* vertexを表すnode */
class node{
  vector<int> from_vector;
  vector<edge> edge_vector;
public:
  vector<int> from_v(){ return from_vector; }
  vector<edge> edge_v(){ return edge_vector; }
  int from_size(){ return from_vector.size(); }
  int edge_size(){ return edge_vector.size(); }
  void add_edge(int dest, int cost){
    edge new_edge(dest, cost);
    edge_vector.push_back(new_edge);
  }
  void set_back(int from){ from_vector.push_back(from); }
};

class graph{
  vector<node> node;
  converter conv;
  int edge_size;
public:
  void read_from_file(char *, converter &);
  void init(int node_num){ node.resize(node_num); edge_size = 0; }  
  int  n_size(){ return node.size(); }
  int  e_size(){ return edge_size; }
  void add_edge(input_line *);
  void dump();
};

void graph::dump(){ 
  cout << "number of node : " << n_size() << endl;
  cout << "number of edge : " << e_size() << endl;
  for(int i = 0; i < n_size(); i++){
    //    cout << i << ", " << conv.itos(i) << " |";
    cout << i << ", ";
    dump_vec(node.at(i).edge_v());
  }
  return;
}

ostream &operator<<(ostream &stream, edge &e){
  stream << e.d() << "(" << e.w() << ")" << " ";
  return stream;
}

input_line *converter::str_to_edge(string str){
  /* string型に格納されたdagfileの1行をparseして構造体をつくる */
  int i = str.find(','), j = str.find(';');
  int from_num = this -> stoi(str.substr(0, i));
  int to_num   = this -> stoi(str.substr(i + 1, j - i - 1));
  int weight   = stoi(str.substr(j + 1));
  input_line *line;
  line  = new input_line;
  line->set(from_num, to_num, weight);
  cout << *line;
  return line;
}

ostream &operator<<(ostream &stream, input_line &i){
  stream << i.from() << "=>" << i.to() << "[" << i.w() << "]" << endl;
  return stream;
}

void graph::add_edge(input_line *line){
  edge_size++;  /* graphにedgeを張る */
  node.at(line->from()).add_edge(line->to(), line->w());
  node.at(line->to()).set_back(line->from());
  return;
}

void graph::read_from_file(char *dagfile, converter &conv){
  ifstream dagfile_fs(dagfile);
  if ( dagfile_fs.fail() ){
    cerr << "cannot open dag file" << endl; exit(1);
  }

  stack<input_line *> input; string buf;
  /* dagfileの内容は一度stackに格納する */
  while(getline(dagfile_fs, buf)){ input.push(conv.str_to_edge(buf)); }
  dagfile_fs.close();

  init(conv.size()); /* 初期化 */

  while(input.size() > 0){ 
    /* edgeの情報を構造体に入れ，stackから取り除く */
    add_edge(input.top()); delete input.top(); input.pop();
  }
  return;
}



int main(int argc, char *argv[]){
  if(argc < 2){
    cerr << "usage: $" << argv[0] 
	 << " <dagfile>" << endl;
  }else{
    char *dagfilepath = argv[1];
    converter conv;
    graph *g = new graph;
    g->read_from_file(dagfilepath, conv);
    
    g->dump();

    cout << "!" << endl;
#if 0
    g->topological_sort();    /* DAGの一列化 */
    cout << "!" << endl;
    g->shortest_path(0);
    g->shortest_path_dump(conv, 0);
    g->dump();
    g->destroy();
#endif
    return 0;
  }
}










#if 0
void graph2::read_from_file(char *dagfile, converter &conv){
  /* dagfileのファイル名を持つファイルの内容を読み取り，
   * graph2構造体にセットする*/
  ifstream dagfile_fs(dagfile);
  if ( dagfile_fs.fail() ){
    cerr << "cannot open dag file" << endl; exit(1);
  }
  string buf;
  stack<string> input_str;
  deque<edge *> input_data;
  /* dagfileの内容は一度queueに格納する */
  while(getline(dagfile_fs, buf)){ /* edge構造体にして格納*/
    input_data.push_back(conv.str_to_edge(buf));  
  }
  dagfile_fs.close();
  /* ファイルを閉じる */

  this -> init(conv.size(), input_data.size()); 
  /* graph2構造体の初期化*/


  /* ここでedgeを追加する際に、srcやsinkのことはとくに考えない
   * 各edgeごとにlistを持ち、さらに終点側が始点へのポインタリストを持つような構造を考える
   * あとでdfsをするときに、トポロジカルソートと連結成分の検出を同時に行う
   *
   */


  vector<bool> src(conv.size()), snk(conv.size());
  /* source と sink を探す */
  for(int i = 0; i < conv.size(); i++){
    src.at(i) = true, snk.at(i) = true;
  }
  while(input_data.size() > 0){ 
    src.at(input_data.front()->to()) = false;
    snk.at(input_data.front()->from()) = false;
    add_edge(input_data.front()); /* graph2構造体にedgeの情報を入れる*/ 
    delete input_data.front();    /* edge のオブジェクトをdeleteする */
    input_data.pop_front();       /* dequeから取り除く */
  }
  for(int i = 0; i < conv.size(); i++){
    /* sourceとsinkをdequeに入れる */
    if(src.at(i) == true) source.push_back(i);
    if(snk.at(i) == true) sink.push_back(i);
  }
  return;
}








class edge{ /* 入力ファイル中のedgeを保存する構造体 */
  int from__; /* 有向グラフの始点 */
  int to__;   /* 有向グラフの終点 */
  int dist__; /* 有向グラフのdistance */
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


class node{
  
};



class graph{
  int edge_size;
  converter conv;
  vector<node> node;
public:
  int v_size(){ return node.size(); };
  int e_size(){ return edge_size; };
};

int main(int argc, char *argv[]){
  if(argc < 2){
    cerr << "usage: $" << argv[0] 
	 << " <dagfile>" << endl;
  }else{
    char *dagfilepath = argv[1];
    converter conv;

#if 0
    graph2 *g = new graph2;
    g->read_from_file(dagfilepath, conv);
    cout << "!" << endl;
    g->topological_sort();    /* DAGの一列化 */
    cout << "!" << endl;
    g->shortest_path(0);
    g->shortest_path_dump(conv, 0);
    g->dump();
    g->destroy();
#endif
    return 0;
  }
}


/*



 */

#if 0
class graph2{
  /* グラフを表す構造体 隣接行列(adjacency matrix) で実装した */
  int **adjmat; /* adjacency matrix */
  int vertex_size__; 
  int edge_size__;
  int *topol;    /* トポロジカルソートの結果 */
  int *dist_sum; /* ある点からの距離の和 */
  int *trbk;     /* 最短路のトレースバック */
  deque<int> source;
  deque<int> sink;
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

inline void graph2::add_edge(edge *e){
  adjmat[e -> from()][e -> to()] = e -> dist();  return;
}

inline void graph2::shortest_path_dump_vertex(converter &conv, int i){
  cout << conv.itos(i)
       << "(" << dist_sum[i] << ")" 
       << " ";
  return;
}

inline void graph2::shortest_path_dump_each(converter &conv, vector<bool> &visited,
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

void graph2::shortest_path_dump(converter &conv, int start){
  /* startから到達可能な点に関して, minimum distanceとpathを表示する */
  vector<bool> visited(vertex_size()); 
  /* visited: すでに表示されたpathの部分列が何度も表示されるのを防ぐための変数 */
  for(int i = 0; i < vertex_size(); i++){ visited.at(i) = false; }
  for(int i = vertex_size() - 1; i >= start; i--){
    if(visited.at(topol[i]) == false){ shortest_path_dump_each(conv, visited, start, topol[i]); }
  }
  return;

}

void graph2::shortest_path(int start){
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


void graph2::topological_sort_rec(int i, int &count, vector<bool> &visited){
  for(int j = 0; j < vertex_size(); j++){
    if(get_dist(i, j) != NO_EDGE){
      if(visited.at(j) == false){
	topological_sort_rec(j, count, visited);
      }else{ /* warning: it's not a DAG */
	cerr << "There exists a cyclic path including a edge "
	     << "(" << i << "," << j << ")" << endl;
        cerr << "This graph2 is NOT a DAG; aborting" << endl;
        exit(1);
      }
    }
  }
  topol[--count] = i;
  return;
}

void graph2::topological_sort_rec(deque<int>source, vector<bool> visited){
  stack<int>to_go;
  vector<bool>visiting;
  /* togoにsourceを入れる*/
  while(to_go.size() > 0){
    int to_go.top(); to_go.pop();
  }
  return;
}
  

void graph2::topological_sort(){
  /* topological sortを実行して, topol [] にindexを格納 */
  if(vertex_size() < 2){ return; } /* arleady sorted */

  vector<bool> visited(vertex_size());
  for(int i = 0; i < vertex_size(); i++){ visited[i] = false; }
  int topological_num = vertex_size();
  cout << topological_num << endl;
  topological_sort_rec(0, topological_num, visited);

#if 0 /* topological sortの結果を表示したい時に1 */
  dump_ary(topol, vertex_size());
#endif
  return;
}

void graph2::dump(){/* class graph2 の objectの内容を表示する */
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

void graph2::read_from_file(char *dagfile, converter &conv){
  /* dagfileのファイル名を持つファイルの内容を読み取り，
   * graph2構造体にセットする*/
  ifstream dagfile_fs(dagfile);
  if ( dagfile_fs.fail() ){
    cerr << "cannot open dag file" << endl; exit(1);
  }
  string buf;   deque<edge *> input_data;
  /* dagfileの内容は一度queueに格納する */
  while(getline(dagfile_fs, buf)){ /* edge構造体にして格納*/
    input_data.push_back(conv.str_to_edge(buf));  
  }
  dagfile_fs.close();
  /* ファイルを閉じる */

  this -> init(conv.size(), input_data.size()); 
  /* graph2構造体の初期化*/


  /* ここでedgeを追加する際に、srcやsinkのことはとくに考えない
   * 各edgeごとにlistを持ち、さらに終点側が始点へのポインタリストを持つような構造を考える
   * あとでdfsをするときに、トポロジカルソートと連結成分の検出を同時に行う
   *
   */


  vector<bool> src(conv.size()), snk(conv.size());
  /* source と sink を探す */
  for(int i = 0; i < conv.size(); i++){
    src.at(i) = true, snk.at(i) = true;
  }
  while(input_data.size() > 0){ 
    src.at(input_data.front()->to()) = false;
    snk.at(input_data.front()->from()) = false;
    add_edge(input_data.front()); /* graph2構造体にedgeの情報を入れる*/ 
    delete input_data.front();    /* edge のオブジェクトをdeleteする */
    input_data.pop_front();       /* dequeから取り除く */
  }
  for(int i = 0; i < conv.size(); i++){
    /* sourceとsinkをdequeに入れる */
    if(src.at(i) == true) source.push_back(i);
    if(snk.at(i) == true) sink.push_back(i);
  }
  return;
}

#endif
#endif

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
