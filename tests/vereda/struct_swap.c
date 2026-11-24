// Swapping the fields of a struct {int a, b;} passed in one i64 register:
// InstCombine makes the swap llvm.fshl.i64 by 32 (found in #121).
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

struct pair { int a, b; };

static struct pair swap(struct pair p) {
  struct pair q = {p.b, p.a};
  return q;
}

int main(void) {
  struct pair p = {__VERIFIER_nondet_int(), __VERIFIER_nondet_int()};
  struct pair q = swap(p);
  __VERIFIER_assert(q.a == p.b && q.b == p.a);
  return 0;
}
