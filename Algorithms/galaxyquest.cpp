#include <bits/stdc++.h>
#include <vector>
#define _CRT_DISABLE_PERFCRIT_LOCKS
#include <stdio.h>
using namespace std;
#define IS_IN_D(x1, x2, y1, y2, d_squared)                              \
  (((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) <= d_squared)

struct Grouping {
  long x, y;
  Grouping(long _x, long _y) : x(_x), y(_y) {}
};

inline void get_coord(unsigned long &storage, const char term) {
  char c;
  storage = 0;
  while ((c = getchar_unlocked()) != term)
    storage = storage * 10 + (c - '0');
}

int main() {
  unsigned long  i, x = 0, y = 0, d = 0;
  unsigned int star_count = 0;
  vector<Grouping> universe;
  int unused __attribute__((unused));
  unused = fscanf(stdin, "%ld %d\n", &d, &star_count);
  
  universe.reserve(star_count);
  for (i = 0; i < star_count; ++i) {
    get_coord(x, ' ');
    get_coord(y, '\n');
    universe.emplace_back(x, y);
  }
  register const unsigned int half = star_count >> 1;
  
  auto winner = universe.begin();
  register const unsigned long d_squared = d*d;
  register unsigned int count = 1;
   i = 0;
  for( auto g = ++universe.begin(); g != universe.end(); ++g, ++i){
    if(IS_IN_D(g->x, winner->x, g->y, winner->y, d_squared)) ++count;
    else --count;
    if(!count){ count = 1; winner = g; }
    if(__builtin_expect((count > half),0)) break;
    if (__builtin_expect((count > half - i),0)) break;
  }
  if (count) {
    count = 0;
    for (auto g : universe)
      count += (__builtin_expect(IS_IN_D(g.x, winner->x, g.y, winner->y, d_squared),1));
      if(__builtin_expect(count > half, 0)) {
          cout << count << "\n";
          return 0;
      }
  }
  cout << "NO" << "\n";
  return 0;
}
