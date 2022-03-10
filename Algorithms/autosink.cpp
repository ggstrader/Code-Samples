#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
using namespace std;
#define NULL_COST 0xffffffff
#define NAME_LEN 20
#define TOLL_LEN 5
#define MAXLINE_LEN (NAME_LEN*2) + DELS
#define DELS 2 // num of delineators per line

//typedefs
typedef uint64_t ulong;
typedef uint32_t uint;
typedef uint16_t uhalf;
typedef const uint64_t ulongc;
typedef const uint32_t uintc;
typedef const uint16_t uhalfc;

typedef vector<unordered_set<uhalf>> AdjL;
typedef vector<uhalf> VecHaf;
typedef unordered_map<string, uhalf> IdTable;
//////

/// IO

#include <unistd.h>
#include <fcntl.h>
///////////////////// Fast IO ////////////////////

template <typename T> inline T get_num(char term) {
  char c;
  T res = 0;
  while ((c = getchar_unlocked()) != term)
    res = res * 10 + (c - '0');
  return res;
}

#define pc_ul(x) putchar_unlocked(x)
//writes all but null terminator
inline void write_str(const char *city) {
  while (*city && pc_ul(*city)) city++;
}

//from Stack overflow for fast number output
template<typename T>
inline void write_num (T num, char term)
{
    T rev, count = 0;
    rev = num;
    if (num == 0) { pc_ul('0'); pc_ul('\n'); return ;}
    while ((rev % 10) == 0) { count++; rev /= 10;} //obtain the count of the number of 0s
    rev = 0;
    while (num != 0) { rev = (rev<<3) + (rev<<1) + num % 10; num /= 10;}  //store reverse of N in rev
    while (rev != 0) { pc_ul(rev % 10 + '0'); rev /= 10;}
    while (count--) pc_ul('0');
    pc_ul(term);
}
//from Stack overflow for fast file read
inline void getstr(char* const __restrict__ buf, uint len) {
    fgets_unlocked(buf, len, stdin);
}
template <typename T> inline T get_num(const char *& __restrict__ dest, char term) {
  char c; T res = 0;
  while ((c = *dest++) != term) res = res * 10 + (c - '0');
  return res;
}
inline string get_str(const char *& __restrict__ dest, char term) {
  string res;
  while (*dest != term) res.push_back(*dest++);
  dest++; // go past term
  return res;
}
#undef pc_ul
/////////////////////////////////////////////////
///

template<typename T>
struct Matrix {
  T* data;
  uint width;
  Matrix(uint w, T fill = 0) : width(w) {
    data = new T[w*w];
    fill_n(data, w*w,fill);
  }
  inline T* operator[](uint i) { return data + (i*width); }
};

inline uint get_min_cost(unordered_set<uhalf>&, vector<uint>&);
inline void explore(uhalf, AdjL&, vector<bool> &, VecHaf &, VecHaf&, uhalf &);
inline void dfs(AdjL &, VecHaf&, VecHaf&);





int main() {
  IdTable cities;
  char buf[MAXLINE_LEN]; const char* bufref;
  // resets and refills input buffer
  #define GETL() getstr(buf,MAXLINE_LEN); bufref = buf;
  posix_fadvise(STDIN_FILENO, 0, 0, 1); // advise kernal of sequential access

  /*
    Assign cities to ids so ids can be used to acces collections
    Provides better locality and access speed than having map of
    strings to Vert objects containing all their data
  */
  const uhalf num_cities = get_num<uhalf>('\n');
  VecHaf tolls(num_cities, 0);
  cities.reserve(num_cities);
  for (uhalf i = 0; i < num_cities; i++) {
    GETL();
    const string city = get_str(bufref, ' ');
    tolls[i] = get_num<uhalf>(bufref, '\n');
    cities[city] = i;
  }

  // construct adjacency adj_lst
  AdjL adj_lst(num_cities);
  const uhalf num_hghwys = get_num<uhalf>('\n');
  for (uhalf i = 0; i < num_hghwys; i++) {
    GETL();
    const string city = get_str(bufref, ' ');
    const string dest = get_str(bufref, '\n');
    adj_lst[cities[city]].insert(cities[dest]);
  }

  // use depth-first search to (reverse) topologically sort
  VecHaf sorted_ids(num_cities);
  VecHaf post_times(num_cities);
  dfs(adj_lst, sorted_ids, post_times);

  struct pack { bool b; uint cache;};
  // use reachable matrix to ignore verts not between source and dest
  Matrix<pack> reachable(num_cities,{0,NULL_COST});
  for(auto id : sorted_ids)
    for(auto c : adj_lst[id]){
      reachable[id][c].b = true;
      for (uhalf e = 0; e < num_cities; e++)
        reachable[id][e].b |= reachable[c][e].b;
    }
  /*
    Calculate lowest child cost for each vert for each trip. Going in reverse
    sort order gaurantees that all childeren of the current vert have been
    calculated.
  */
  const uhalf num_trips = get_num<uhalf>('\n');
  vector<uint> costs_so_far(num_cities);
  for (uhalf i = 0; i < num_trips; i++) {
    GETL();
    const string city = get_str(bufref, ' ');
    const string dest = get_str(bufref, '\n');
    if (city == dest) { write_num<int>(0, '\n'); continue; }
    const uhalf cid = cities[city];
    const uhalf did = cities[dest];
    const uhalf ct_pst = post_times[cid];
    uhalf dt_pst = post_times[did];

    if (ct_pst < dt_pst || !reachable[cid][did].b) { write_str("NO\n"); continue; }
    // don't process all verts if dest is immediate child
    if (adj_lst[cid].find(did) != adj_lst[cid].end())
      {write_num<uint>(tolls[did], '\n'); continue;}
    if(reachable[cid][did].cache != NULL_COST)
    {write_num<uint>(reachable[cid][did].cache - tolls[cid], '\n'); continue;}
    fill(costs_so_far.begin(), costs_so_far.end(), NULL_COST);
    costs_so_far[did] = (uint)tolls[did];
    uint res;
    for (dt_pst++; dt_pst < ct_pst; dt_pst++) {
      const uhalf &curr = sorted_ids[dt_pst];
      if(!reachable[cid][curr].b) continue;
      if(!reachable[curr][did].b) continue;
      if (reachable[curr][did].cache == NULL_COST) {
        res = get_min_cost(adj_lst[curr], costs_so_far) + tolls[curr];// avoid at all costs
        reachable[curr][did].cache = res;
      } else res = reachable[curr][did].cache;
      costs_so_far[curr] = res;
    }
    res = get_min_cost(adj_lst[sorted_ids[dt_pst]], costs_so_far);
    reachable[cid][did].cache = res + tolls[cid];
    if (res == NULL_COST) write_str("NO\n");
    else write_num<uint>(res, '\n');
  }
  return 0;
}





//////////////////////////////////////////////////////////////////


void explore(uhalf curr, AdjL &adjl, vector<bool> &visited,
               VecHaf &sorted_ids, VecHaf &post_times,
               uhalf &clock) {
  if (visited[curr]) return;
  visited[curr] = true;
    for (auto edge : adjl[curr])
      explore(edge, adjl, visited, sorted_ids, post_times, clock);
  sorted_ids[clock] = curr;
  post_times[curr] = clock++;
}

void dfs(AdjL &adjl, VecHaf &sorted_ids, VecHaf &post_times) {
  uhalf clock = 0;
  vector<bool> visited(adjl.size(), 0);
  for (int i = 0; i < adjl.size(); i++)
    explore(i, adjl, visited, sorted_ids, post_times, clock);
}

/*
  Since my algorithm ignores impossible paths, every vert this function is
  called on will have at least one child with a cost so far, since it must
  connect to the destination. Hence, this function never returns NULL_COST,
  but would in other implementions.
*/
inline uint get_min_cost(unordered_set<uhalf> &children, vector<uint> &csf) {
  int min = NULL_COST;
  for (auto e : children) {
    uint &c = csf[e];
    if (c < min) min = c;
  }
  return min;
}
