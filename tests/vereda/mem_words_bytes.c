// Arrays indexed at a symbolic offset that are not read and written only as
// whole integers of one width keep arrays of bytes (#107), and stay right:
// one read as chars too, one filled by memset, one read at an offset that is
// not a multiple of its element, one read as shorts too.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4 && j < 4);

  int c[4];
  c[i] = 0x01020304;
  __VERIFIER_assert(((unsigned char *)c)[4 * i + 1] == 3);

  int m[4];
  memset(m, 0, sizeof m);
  m[i] = 5;
  __VERIFIER_assert(m[i] == 5 && (i == j || m[j] == 0));

  int u[4];
  unsigned k = i % 3;
  u[k] = 0x01020304;
  u[k + 1] = 0x05060708;
  __VERIFIER_assert(*(int *)((char *)u + 4 * k + 2) == 0x07080102);

  int x[4];
  x[i] = 0x00050006;
  __VERIFIER_assert(((short *)x)[2 * i + 1] == 5 && ((short *)x)[2 * i] == 6);
  return 0;
}
