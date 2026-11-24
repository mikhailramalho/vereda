// The three-way compare (x > y) - (x < y), which clang turns into llvm.scmp
// for signed operands and llvm.ucmp for unsigned ones.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

static int scmp(int x, int y) { return (x > y) - (x < y); }
static int ucmp(unsigned x, unsigned y) { return (x > y) - (x < y); }

int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  int r = scmp(x, y);
  __VERIFIER_assert((r == -1) == (x < y) && (r == 1) == (x > y) && (r == 0) == (x == y));
  unsigned u = __VERIFIER_nondet_uint(), v = __VERIFIER_nondet_uint();
  int q = ucmp(u, v);
  __VERIFIER_assert((q == -1) == (u < v) && (q == 1) == (u > v) && (q == 0) == (u == v));
  return 0;
}
