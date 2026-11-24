// Arrays read and written only as whole integers of one width, some at a
// symbolic index, get arrays of such elements (#107): int, short and long,
// global and local, zero-filled, initialised and not, written at constant
// and symbolic indices, on both sides of a branch and in a loop.
// RUN: --unwind 65
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
int gi[2048];
short gs[6] = {1, -2, 3, -4, 5, -6};
long gl[3];

int main(void) {
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 6 && j < 6);

  __VERIFIER_assert(gi[i] == 0);
  gi[i] = 7;
  gi[3] = 9;
  __VERIFIER_assert(gi[i] == (i == 3 ? 9 : 7));
  __VERIFIER_assert(gi[j] == (j == 3 ? 9 : j == i ? 7 : 0));
  __VERIFIER_assert((&gi[i] == &gi[j]) == (i == j));

  __VERIFIER_assert(gs[i] == (i % 2 ? -(short)(i + 1) : (short)(i + 1)));
  gs[j] = -300;
  __VERIFIER_assert(gs[j] == -300);
  __VERIFIER_assert(i == j || gs[i] == (i % 2 ? -(short)(i + 1) : (short)(i + 1)));

  gl[i % 3] = -1L;
  __VERIFIER_assert(gl[i % 3] == -1L && gl[(i + 1) % 3] == 0);

  int l[4];
  l[j % 4] = 1;
  if (__VERIFIER_nondet_int())
    l[i % 4] = 2;
  else
    l[2] = 3;
  int v = l[j % 4];
  __VERIFIER_assert(v == 1 || v == 2 || v == 3);
  __VERIFIER_assert(v != 3 || j % 4 == 2);

  for (int k = 0; k < 64; k++)
    gi[k] = 2 * k;
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x < 2048);
  __VERIFIER_assert(gi[x] == (x < 64 ? 2 * x : 0));
  return 0;
}
