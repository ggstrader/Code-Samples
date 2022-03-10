#include <bits/stdc++.h>
#include <stdio.h>
#include <string>
#include <unordered_set>
using namespace std;

//########################################################################
/*NOTE: Hash function for int[] from stack overflow. Used only for capability
 of having a set of int[], doesn't really affect algorithm complexity*/
namespace std {
template <typename T, size_t N> struct hash<array<T, N>> {
  typedef array<T, N> argument_type;
  typedef size_t result_type;

  result_type operator()(const argument_type &a) const {
    hash<T> hasher;
    result_type h = 0;
    for (result_type i = 0; i < N; ++i) {
      h = h * 31 + hasher(a[i]);
    }
    return h;
  }
};
} // namespace std
//#########################################################################

int main() {
  short n, k, i, j;
  fscanf(stdin, "%hd %hd\n", &n, &k);
  unordered_set<array<char, 26>> pass;
  unordered_set<array<char, 26>> fail;
  array<char, 26> alph;

  for (i = 0; i < n; i++) {
    alph.fill(0); 
    for (j = 0; j < k; j++) alph[getchar_unlocked() - 'a']++;
    getchar_unlocked(); // skip newline

    if (pass.erase(alph) == 1) fail.insert(alph);
    else if (fail.find(alph) == fail.end())
      pass.insert(alph);
  }

  cout << pass.size() << '\n';
  return 0;
}
