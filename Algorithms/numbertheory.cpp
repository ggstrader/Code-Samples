#define MAX_NUM_BITS 31
#define MAX_NUM 2
#define MAX_INST 8
#define MAX_DELS 3
#define MAXLINE_LEN ((MAX_NUM_BITS*MAX_NUM) + MAX_INST + MAX_DELS)
#define INTLEN 50

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

typedef uint64_t ulong;
typedef uint32_t uint;
typedef uint16_t uhaf;
using namespace std;

///////////////////// Fast IO ////////////////////

namespace FastIo {
  static const uint buff_size = 4096*8;
  static const char EOT = '\003';
  char input[buff_size + 1] = {};
  char output[buff_size] = {};
  uint i_current = 0;
  uint i_total = 0;
  uint o_total = 0;

  void init_input() {
    input[buff_size] = EOT;
    struct stat stats;
    fstat(STDIN_FILENO, &stats);
    setvbuf(stdin, NULL, _IONBF, stats.st_blksize);
    setvbuf(stdout, NULL, _IONBF, stats.st_blksize);
    posix_fadvise(STDIN_FILENO, 0, 0, 1); // advise kernal of sequential access
  }

  static inline void check_and_reset_output(uint maxwrite) {
    if (o_total + maxwrite > buff_size || maxwrite == 0) {
      syscall(SYS_write, STDOUT_FILENO, output, o_total);
      o_total = 0;
    }
  }
  /*
   If reading from input exceeds available input, copy unread data back
  to the start of input and fill the buffer again
   Returns whether current read will reach EOT
  */
  static inline void check_and_reset_input(uint maxread) {
    // i_total will be 1 past the last byte so make sure we don't read at s.i_total
    if (i_current + maxread >= i_total) {
      const int leftovers = i_total - i_current;
      char *unread = &input[i_current];
      for (int i = 0; i < leftovers; i++)
      input[i] = unread[i];
      i_total = syscall(SYS_read, STDIN_FILENO, &input[leftovers], buff_size - leftovers) + leftovers;
      input[i_total] = EOT;
      i_current = 0;
    }
  }


  template <typename T> static inline T get_num(char term) {
    check_and_reset_input(INTLEN);
    T res = 0;
    while (__builtin_expect(input[i_current] != term, true) && __builtin_expect(input[i_current] != EOT, true))
      res = (res<<1) + (res<<3) + input[i_current++] - '0';
    if(input[i_current] != EOT) i_current++;//increment past term
    return res;
  }


  static inline void write_str(const char *str) {
    check_and_reset_output(MAXLINE_LEN);
    while (*str) output[o_total++] = *str++;
  }


  template<typename T>
  static inline void write_num (T num, char term)
  {
    check_and_reset_output(INTLEN);
    T rev, count = 0;
    rev = num;
    if (num == 0) { output[o_total++] = '0'; output[o_total++] = term; return ;}
    while ((rev % 10) == 0) { count++; rev /= 10;} //obtain the count of the number of 0s
    rev = 0;
    while (num != 0) { rev = (rev<<3) + (rev<<1) + num % 10; num /= 10;}  //store reverse of N in rev
    while (rev != 0) { output[o_total++] = (rev % 10 + '0'); rev /= 10;}
    while (count--) output[o_total++] = '0';
    output[o_total++] = term;
  }
  static inline bool MoreIo() {
    if(i_total - i_current < 1)
      check_and_reset_input(buff_size);
    return i_total > 0;
  }

  static inline char PeekByte(int offset = 0){
    return input[i_current + offset];
  }

  static inline void ConsumeN(int n = 0) {
    i_current += n;
  }
}




/////////////////////// Problem //////////////////////////

inline void key();
inline void isprime();
inline void inverse();
inline long inverse(long, long);
inline void exp();
inline ulong exp(ulong, ulong, ulong);
inline ulong gcd(ulong, ulong);
inline void gcd();
using namespace FastIo;

int main() {
  init_input();
  check_and_reset_input(buff_size);
  while (MoreIo()) {
    char c = PeekByte();
    switch (c) {
      case 'g': gcd(); break;
      case 'e': exp(); break;
      case 'k': key(); break;
      case 'i':
        c = PeekByte(1);
        switch (c) {
          case 'n': inverse(); break;
          case 's': isprime(); break;
        }
    }
  }
  check_and_reset_output(buff_size);
  return 0;
}

template<typename T>
inline T mod(T x, T m) {
  return x >= m ? x % m : x;
}
//adapted from SO
inline  ulong gcd(ulong a, ulong b) {
  if( a<0 ) a = -a;
    if( b<0 ) b = -b;
    while( b!=0 ){
        a %= b;
        if( a==0 ) return b;
        b %= a;
    }
  return a;
}

#define GCD_STR_LEN 4
void gcd() {
  ConsumeN(GCD_STR_LEN);
  ulong a = get_num<ulong>(' '), b = get_num<ulong>('\n');
  write_num(gcd(a,b), '\n');
}

inline  uint gbp(uint n) {
 uint m = n;
 m = m | m >> 1;
 m = m | m >> 2;
 m = m | m >> 4;
 m = m | m >> 8;
 m = m | m >> 16;
 m = m & ((~m >> 1)^0x80000000);
return m;
}

inline  ulong exp(ulong x, ulong y, ulong n) {
  ulong z = 1;
  for (ulong i = gbp(y); i > 0; i >>= 1) {
    if(i&y) z = mod(x*mod(z*z, n), n);
    else z = mod(z*z, n);
  }

  return z;
}
#define EXP_STR_LEN 4
void exp() {
  ConsumeN(EXP_STR_LEN);
  ulong x = get_num<ulong>(' '), y = get_num<ulong>(' '), n = get_num<ulong>('\n');
  write_num(exp(x,y,n), '\n');
}

inline  long inverse(long a, long N){
  long t = 0, newt = 1;
  long r = N, newr = a;
   while (newr != 0) {
       const auto quotient = r /newr;
       const auto temp = newt;
       newt = t- quotient * newt;
       t = temp;
       const auto temp2 = newr;
       newr = r - quotient * newr;
       r = temp2;
   }
   if (r > 1) return -1;
   if (t < 0) t += N;
   return t;
}
#define INV_STR_LEN 8
void inverse() {
  ConsumeN(INV_STR_LEN);
  long a = get_num<long>(' '), N = get_num<long>('\n');
  long res = inverse(a,N);
  res == -1 ? write_str("none\n") :  write_num(res, '\n');
}

#define PRM_STR_LEN 8
void isprime2( ulong p1) {
  ConsumeN(PRM_STR_LEN);
  ulong p2 = get_num<ulong>('\n');
  // if(PeekByte() == 'i' && PeekByte(1) == 's')
  //   {isprime3(p1, p2); return;}
  ulong z1 = 1;
  ulong z2 = 1;
  for (ulong i = 0x80000000; i > 0; i >>= 1) {
    if(i&(p1-1)) z1 = mod(2*mod(z1*z1, p1), p1);
    else z1 = mod(z1*z1, p1);
    if(i&(p2-1)) z2 = mod(2*mod(z2*z2, p2), p2);
    else z2 = mod(z2*z2, p2);
  }
  write_str((z1 == 1 ? "yes\n" : "no\n"));
  write_str((z2 == 1 ? "yes\n" : "no\n"));
}


inline  void isprime() {
  ConsumeN(PRM_STR_LEN);
  ulong p = get_num<ulong>('\n');
  if(PeekByte() == 'i' && PeekByte(1) == 's')
    {isprime2(p); return;}
  if( p < 3 || p & 0)
    write_str("no\n");
  //uncomment for significantly better odds of primeness
  else if(exp(2, p-1, p) == 1) //&& exp(3, p-1, p) == 1 )// && exp(5, p-1, p) == 1)
    write_str("yes\n");
  else write_str("no\n");
}


#define KEY_STR_LEN 4
inline  void key() {
  ConsumeN(KEY_STR_LEN);
  const ulong p = get_num<ulong>(' '), q = get_num<ulong>('\n');
  static int ecands[] = {3,5,7,9,11,13,17,19,23,27,29,31};
  ulong i = 1, pbexp = ecands[0], modulus = p*q, phi = (p-1)*(q-1);
  while(gcd(pbexp,phi) != 1) pbexp = ecands[i++];
  ulong prexp = (ulong)inverse(pbexp,phi);
  write_num(modulus, ' ');
  write_num(pbexp, ' ');
  write_num(prexp, '\n');
}
