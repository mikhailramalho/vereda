// RUN: --no-summarize-array-loops
// RUN: --no-summarize-array-loops -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct entry { unsigned char kind; unsigned short len; };
int main(void) {
  struct entry a[256];
  for (unsigned i = 0; i < 256; ++i) {
    a[i].kind = i % 7;
    a[i].len = 3 * i;
  }
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 256);
  __VERIFIER_assert(a[i].kind == i % 7);
  __VERIFIER_assert(a[i].len == 3 * i);
  a[i].len = 42;
  __VERIFIER_assert(a[i].kind == i % 7);
  __VERIFIER_assert(a[i].len == 42);
}
