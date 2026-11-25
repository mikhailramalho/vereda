// A failing assertion against a large splat non-zero constant array.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
struct T {
  int head;
  int vals[2048];
  int tail;
};
static struct T g = { 5, { [0 ... 2047] = 0x01020304 }, 9 };
int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 2048);
  __VERIFIER_assert(g.vals[i] == 0);
  return 0;
}
