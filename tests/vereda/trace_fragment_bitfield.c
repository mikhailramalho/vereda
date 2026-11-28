// SROA splits s, but its bitfields share one byte, whose debug record
// covers no single member: the counterexample shows s.c and nothing for
// the bitfields (#311).
// EXPECT: ^  s\.c = 25;
// EXPECT-NOT: ^  s(\.a|\.b)? =
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
struct B {
  unsigned a : 3;
  unsigned b : 5;
  int c;
};

int main(void) {
  int k = __VERIFIER_nondet_int();
  struct B s;
  s.a = 1;
  s.b = 2;
  s.c = k * k;
  __VERIFIER_assert(s.a + s.b + s.c != 3 + 25);
  return 0;
}
