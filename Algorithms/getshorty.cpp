
#include <bits/stdc++.h>
#include <limits>
#include <unordered_map>

#define AVERAGE_VALENCE 30 // using preprocessor to allow quick changes from cmd line in testing
#define MAXSHORT 0xFFFF

using namespace std;
typedef uint16_t uhalf;
const auto MAXFLT = numeric_limits<float>::max();

////////////////////////////Debug//////////////////////////
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

///////////////////// Fast IO ////////////////////
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
using namespace std;

template <typename T> inline T get_num(char terminator) {
  char c;
  T best = 0;
  while ((c = getchar_unlocked()) != terminator)
  best = (best<<1) + (best<<3) + c - '0';
  return best;
}
// significantly faster for inputing fixed floating point numbers
float get_fixed(char terminator) {
  uint16_t rez = 0, fact = 1;
  char curr;
  if ((curr = getchar_unlocked()) == '1') {
    while ((curr = getchar_unlocked()) != terminator) {}
    return 1.0;
  }
  getchar_unlocked();
  while ((curr = getchar_unlocked()) != terminator) {
    fact = (fact<<3) + (fact<<1);
    rez = (rez<<3) + (rez<<1) + (uint16_t)(curr - '0');
  }
  return (float)rez / (float)fact;
}

#define pc_ul(x) putchar_unlocked(x)
//writes all but null terminator
inline void write_str(const char *str) {
  while (*str && pc_ul(*str)) str++;
}

inline void getstr(char* buffer) {
    while ((*buffer = getchar_unlocked()) != '\n') buffer++;
}

inline string get_str(const char *& __restrict__ from, char terminator) {
  auto* start = from;
  while (*from++ != terminator);
  return string(start, from-1);//-1 to exclude terminator
}

template<typename T>
inline void write_num (T num, char terminator)
{
    T rev, count = 0;
    rev = num;
    if (num == 0) { pc_ul('0'); pc_ul('\n'); return ;}
    while ((rev % 10) == 0) { count++; rev /= 10;} //obtain the count of the number of 0s
    rev = 0;
    while (num != 0) { rev = (rev<<3) + (rev<<1) + num % 10; num /= 10;}  //store reverse of N in rev
    while (rev != 0) { pc_ul(rev % 10 + '0'); rev /= 10;}
    while (count--) pc_ul('0');
    pc_ul(terminator);
}

#undef pc_ul

///////////////////// Graph Declaration ////////////////////

class Graph {
public:
  Graph(uhalf);
  float BidirectionalDijkstra();
  void inline Insert(uhalf, uhalf, float);
  void Clear(uhalf);

private:
  struct comparator;
  typedef vector<float> Dists;
  typedef pair<uhalf, float> IDVal;
  typedef vector<IDVal> vIdVal;
  typedef vector<array<IDVal, AVERAGE_VALENCE>> AdjacencyList;
  typedef unordered_map<uhalf, vector<IDVal>> FallbackList;
  typedef priority_queue<IDVal, deque<IDVal>, comparator> PriorityQueue;
  struct comparator
  {
    bool operator()(const IDVal &l, const IDVal &r) { return l.second < r.second; }
  };
  AdjacencyList adj_list;
  FallbackList fallback;
  Dists for_dist, rev_dist;
  PriorityQueue forward_Q, reverse_Q;
  vector<bool> visited;
  vector<uhalf> valencies;
  float best = 0.0;
  inline uhalf dijkstra_step(PriorityQueue &, Dists &);
};

///////////////////// Problem ////////////////////

int main() {
  auto num_nodes = get_num<uhalf>(' ');
  auto num_edges = get_num<uhalf>('\n');
  char arr[8];

  while (num_nodes | num_edges) {
    // populate the graph
    Graph graph(num_nodes);
    for (int i = 0; i < num_edges; i++)
    {
      uhalf start = get_num<uhalf>(' '), end = get_num<uhalf>(' ');
      float factor = get_fixed('\n');
      graph.Insert(start, end, factor);
    }
    // solve
    sprintf(arr, "%.4f\n", graph.BidirectionalDijkstra());
    write_str(arr);
    // check for next test case
    num_nodes = get_num<uhalf>(' ');
    num_edges = get_num<uhalf>('\n');
  }
  return 0;
}


// performing Dijkstra from both the start and end nodes is roughly twice as fast
float Graph::BidirectionalDijkstra() {
  while (!forward_Q.empty() && !reverse_Q.empty()) {
    auto id = dijkstra_step(forward_Q, for_dist);
    if (id != MAXSHORT) {
      if (visited[id]) break; // as we explore the "shortest" path, we know that later visits will be "longer"
      visited[id] = true;
      if (rev_dist[id] != MAXFLT && best < for_dist[id] * rev_dist[id]) // this can only be true if forward and reverse have met
        best = for_dist[id] * rev_dist[id];
    }
    id = dijkstra_step(reverse_Q, rev_dist);
    if (id != MAXSHORT) {
      if (visited[id]) break; // same as above
      visited[id] = true;
      if (for_dist[id] != MAXFLT && best < for_dist[id] * rev_dist[id]) // same as above
        best = for_dist[id] * rev_dist[id];
    }
  }
  return best;
}


inline uhalf Graph::dijkstra_step(PriorityQueue &pqueue, Dists &distances) {
  auto current = pqueue.top();
  pqueue.pop();
  if (current.second != distances[current.first]) return MAXSHORT; // priority queue doesn't support updates, so ignore old values
  if (best != 0.0 && current.second < best) return MAXSHORT; // a lower number will be strictly worse since we're maximizing the product

  for (auto &[id, val] : adj_list[current.first])
    if (distances[id] == MAXFLT || distances[id] < distances[current.first] * val) {
      distances[id] = distances[current.first] * val;
      if (best != 0.0 && distances[id] < best) continue; // no need to expand the node if it's worse than the current best
      pqueue.push(IDVal(id, distances[id]));
    }
  // do the same for the fallback list (if concepts were allowed I'd get rid of this duplication)
  if(valencies[current.first] >= AVERAGE_VALENCE){
  for (auto &[id, val] : fallback[current.first])
    if (distances[id] == MAXFLT || distances[id] < distances[current.first] * val) {
      distances[id] = distances[current.first] * val;
      if (best != 0.0 && distances[id] < best) continue;
      pqueue.push(IDVal(id, distances[id]));
    }
  }
  return current.first;
}


// given the fact that most graphs are sparse, we use a fallback list in the case of a node with more than the AVERAGE_VALENCE
// this allows the use of one contiguous block for the bulk of the adjacency list, which results in better locality and far fewer allocations
void inline Graph::Insert(uhalf start, uhalf end, float weight) {
  auto valence = valencies[start];
  if(valence < AVERAGE_VALENCE) adj_list[start][valence] = {end, weight};
  else fallback[start].push_back({end, weight});
  valencies[start]++;

  valence = valencies[end];
  if(valence < AVERAGE_VALENCE) adj_list[end][valence] = {start, weight};
  else fallback[end].push_back({start, weight});
  valencies[end]++;
}




/////////////////////////////////////////////////

Graph::Graph(uhalf sz) : adj_list(sz), for_dist(sz, MAXFLT), rev_dist(sz, MAXFLT), visited(sz, false), valencies(sz, 0) {
  for_dist[0] = 1.0;
    rev_dist[sz - 1] = 1.0;
    forward_Q.push(IDVal(0, 1.0));
    reverse_Q.push(IDVal( sz-1, 1.0));
}

void Graph::Clear(uhalf new_size) {
  if (new_size == 0) return;
  for_dist.assign(new_size, MAXFLT);
  rev_dist.assign(new_size, MAXFLT);
  visited.assign(new_size, false);
  valencies.assign(new_size, 0);
  fallback.clear();
  forward_Q = PriorityQueue();
  reverse_Q = PriorityQueue();
  adj_list = AdjacencyList(new_size);
  for_dist[0] = 1.0;
  rev_dist[new_size - 1] = 1.0;
  forward_Q.push(IDVal(0, 1.0));
  reverse_Q.push(IDVal(new_size - 1, 1.0));
  best = 0.0;
}