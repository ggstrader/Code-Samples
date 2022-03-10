#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
#define DBG 1

#define print(...) if (DBG) (std::cout << __VA_ARGS__ << std::endl)
#define ENTERED() print ("Entered function: " << __func__)
#define EXITED()  print ("Exited function: " << __func__)
#define HERE(i)   print ("HERE " <<  i)
#define get_type(x) (abi::__cxa_demangle(typeid(x).name(), NULL, NULL, NULL))
#define watch(x)  print ('(' << get_type(x) << ") " << (#x) << " is: " << x)







#include <iostream>
#include <limits>
#include <set>
#include <vector>
#include <numeric>
#include <cstring>
using namespace std;

const int MAXINT = numeric_limits<int>::max();

template<typename T>
struct Matrix;
ostream &operator<<(ostream &os, Matrix<int> & mc);

template<typename T>
struct Matrix {
  friend ostream &operator<<(ostream &os, Matrix<int> & mc);
private:
  T* data;
  uint dim;
  Matrix & _this = (*this);
public:
  const T MAX = numeric_limits<T>::max();

  Matrix(uint d, T fill = 0) : dim(d) {
    data = new T[d*d];
    fill_n(data, d*d,fill);
  }
  Matrix(const Matrix& other) : Matrix(other.dim) {
    memcpy(data, other.data, dim*dim*sizeof(T));
  }
  ~Matrix() {
    delete[] (T*)data;
  }
  inline T* operator[](uint i) const { return data + (i*dim); }

  T Reduce() {
    T cost = 0;
    int j;
    for (int i = 0; i < dim; i ++){
      T curr_min = MAX;
      for (j = 0; j < dim; j++)
        curr_min = min(curr_min, _this[i][j]);
      if(!curr_min || curr_min == MAX) continue;
      for (j--; j >= 0; j--)
        if(_this[i][j] != MAX)
          _this[i][j] -= curr_min;
      cost += curr_min;
    }

    for (int i = 0; i < dim; i ++){
      T curr_min = MAX;
      for (j = 0; j < dim; j++)
        curr_min = min(curr_min, _this[j][i]);
      if(!curr_min || curr_min == MAX) continue;
      for (j--; j >= 0; j--)
        if(_this[j][i] != MAX)
          _this[j][i] -= curr_min;
      cost += curr_min;
    }
    return cost;
  }
  void ElimRowCol(uint row, uint col){
    for (uint i = 0; i < dim; i ++)
      _this[row][i] = MAX;
    for (uint i = 0; i < dim; i ++)
      _this[i][col] = MAX;
  }
  void ElimItem(uint row, uint col) {
    _this[row][col] = MAX;
  }
};

ostream &operator<<(ostream &os, Matrix<int> & mc) {
  os << "\n[\n";
  for(uint i = 0; i < mc.dim; i++) {
    for(uint j = 0; j < mc.dim; j++)
      if(mc[i][j] == mc.MAX)
        os << "\t , ";
      else os << '\t' << mc[i][j] << ", ";
    os << '\n';
  }
  os << "\n]\n";
  return os;
}


int V;
static int bestSoFar = MAXINT;


struct Sol {
  int cost;
  int graph_id;
  vector<int> todo;
  int id;
  bool operator==(const Sol& other) {
    return cost == other.cost && graph_id == other.graph_id;
  }
  bool operator< (const Sol& other) const {
    if(cost == other.cost) return graph_id > other.graph_id;
    return cost > other.cost;
  }
};



void TSP() {
  cin >> V;
  Matrix<int> graph(V);
  vector<Matrix<int>*> graphs;
  set<Sol> unprocessed;


  for (int i = 0; i < V; i++)
    for (int j = 0; j < V; j++) {
      cin >> graph[i][j];
      if(i==j) graph.ElimItem(i, j);
    }

  vector<int> first_todo(V) ;
  iota(begin(first_todo), end(first_todo), 0);
  graphs.push_back(&graph);
  unprocessed.insert({graph.Reduce(), 0, first_todo, 0});

  auto it = unprocessed.end();
  // print("\n\nAfter:");
  while(1) {
    it = prev(unprocessed.end());
    auto curr = *it;
    unprocessed.erase(it);


    int num_processed = 0;
    for (auto i : curr.todo) {
      if(!i) continue;
      else num_processed++;
      auto& cg = *graphs[curr.graph_id];
      Matrix<int>* next = new Matrix<int>(cg);

      next->ElimRowCol(curr.id,i);
      for(auto unvisited : curr.todo)
        if(!unvisited)
          next->ElimItem(i,unvisited);

      graphs.push_back(next);

      auto temp_todo(curr.todo);
      temp_todo[i] = 0;
      int t = curr.cost +  cg[curr.id][i] + next->Reduce();
      // if(t <= 0) abort();
      unprocessed.insert({
        t,
        (int)graphs.size()-1,
        temp_todo,
        i
      });
    }
    if(!num_processed) break;
  }

  cout << (*it).cost  << endl;
  graphs[0] = NULL; // first graph on stack, deleted at scope
  for(auto p : graphs)
    delete p;
}




int main(int argc, char const *argv[]) {
  std::ios::sync_with_stdio(false);
  cin.tie(NULL);


  TSP();

  // for(auto i : todo){
  //   watch(i.val);
  //   watch(*i.tie);
  // }

  // watch(i);
  // watch(next.id);
  // watch(tcost);
  // watch(*temp);

  //
  // print("Cost: " << cost);

  return 0;
}
