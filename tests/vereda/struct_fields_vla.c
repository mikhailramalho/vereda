// RUN: --no-summarize-array-loops --unwind 9
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct entry { unsigned char x; unsigned short y; };
int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n <= 8);
  struct entry a[n];
  for (unsigned i = 0; i < n; ++i) {
    a[i].x = i;
    a[i].y = 3 * i;
  }
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < n);
  __VERIFIER_assert(a[i].x == i);
  __VERIFIER_assert(a[i].y == 3 * i);
}
