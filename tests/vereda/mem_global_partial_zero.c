// A large zeroed sub-array inside a non-zero global struct uses a partial
// fill lambda while preserving the surrounding fields.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
struct S {
  int a;
  int zeros[4096];
  int b;
};
static struct S g = { 42, {0}, 99 };
int main(void) {
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4096 && j < 4096);
  __VERIFIER_assert(g.a == 42 && g.b == 99 && g.zeros[0] == 0 &&
                    g.zeros[4095] == 0 && g.zeros[i] == 0);
  g.zeros[j] = 7;
  __VERIFIER_assert(g.a == 42 && g.b == 99 && g.zeros[j] == 7 &&
                    (i == j || g.zeros[i] == 0));
  return 0;
}
