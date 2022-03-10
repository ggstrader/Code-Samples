#define MAXINT 0xFFFF
#define NAME_LEN 20
#define DELS 2 // num of delineators per line
#define MAXLINE_LEN (NAME_LEN*2) + DELS

#include <string>
#include <unistd.h>
#include <fcntl.h>

#include <queue>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>
using namespace std;

struct pak;
typedef uint64_t ulong;
typedef uint32_t uint;
typedef uint16_t uhalf;
typedef const uint64_t ulongc;
typedef const uint32_t uintc;
typedef const uint16_t uhalfc;

typedef vector<uhalf> VecHaf;
typedef unordered_map<string, uhalf> IdTable;
typedef vector<VecHaf> AdjL;

///////////////////// Fast IO ////////////////////

template <typename T> inline T get_num(char term) {
  char c;
  T res = 0;
  while ((c = getchar_unlocked()) != term)
  res = (res<<1) + (res<<3) + c - '0';
  return res;
}

#define pc_ul(x) putchar_unlocked(x)
//writes all but null terminator
inline void write_str(const char *str) {
  while (*str && pc_ul(*str)) str++;
}

inline void getstr(char* buf) {
    while ((*buf = getchar_unlocked()) != '\n') buf++;
}

inline string get_str(const char *& __restrict__ dest, char term) {
  auto* start = dest;
  while (*dest++ != term);
  return string(start, dest-1);//-1 to exclude term
}

#undef pc_ul

//////////////////////// Problem /////////////////////////

struct pak {
  string s;
  uhalf id;
  pak(string _s = "", uhalf i = 0) : s(_s), id(i) {}
  bool operator< (const pak& r) {return s < r.s;}
};


int main() {
  char buf[MAXLINE_LEN]; const char* bufref;
  // resets and refills input buffer
  #define GETL() getstr(buf); bufref = buf;
  posix_fadvise(STDIN_FILENO, 0, 0, 1); // advise kernal of sequential access

  const auto num_names = get_num<uhalf>('\n');

  IdTable ids(num_names << 2);
  vector<pak> lex_order;
  lex_order.reserve(num_names);


  for (auto i = 0; i < num_names; i++) {
    GETL();
    const auto name = get_str(bufref, '\n');
    ids[name] = i;
    lex_order.emplace_back(name, i);
  }
  sort(lex_order.begin(), lex_order.end());

  //remap ids to their lexicographic order and use that as ids instead
  for (uhalf pos = 0; pos < num_names; pos++)
    ids[lex_order[pos].s] = pos;


  const auto num_pairs = get_num<uhalf>('\n');
  AdjL adj_lst(num_names);
  for (auto i = 0; i < num_pairs; i++) {
    GETL();
    const auto& first = ids[get_str(bufref, ' ')];
    const auto& second = ids[get_str(bufref, '\n')];
    adj_lst[first].push_back(second);
    adj_lst[second].push_back(first);
  }
  //depths ordered lexicographically
  VecHaf depths(num_names, MAXINT);
  unordered_map<uhalf,VecHaf> dpth_toIds;
  const auto num_trips = get_num<uhalf>('\n');
  queue<uhalf> todo;

  for (uhalf i = 0; i < num_trips; i++) {
    GETL();
    const auto& start = ids[get_str(bufref, '\n')];
    todo.push(start);

    depths[start] = 0;
    uhalf max_depth = 0;
    while(!todo.empty()){
      const auto c = todo.front();
      uhalf depth = depths[c] + 1;
      if(depth > max_depth) max_depth = depth;
      todo.pop();
      for(auto edge : adj_lst[c]){
        if(depths[edge] != MAXINT) continue;
        todo.push(edge);
        depths[edge] = depth;
      }
    }
    for(uhalf i = 0; i < num_names; i++){
      dpth_toIds[depths[i]].push_back(i);
    }
    for(uhalf i = 0; i <= max_depth; i++)
      for(auto lex : dpth_toIds[i])
        {write_str( lex_order[lex].s.c_str()); putchar_unlocked(' ');}
    for(auto lex : dpth_toIds[MAXINT])
      {write_str( lex_order[lex].s.c_str()); putchar_unlocked(' ');}
    write_str("\n");
    fill(depths.begin(), depths.end(), MAXINT);
    dpth_toIds.clear();
  }
  return 0;
}
