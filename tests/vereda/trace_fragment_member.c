// SROA splits t into its two fields, each a value with a debug record
// for its part of t: the counterexample names the field whose value comes
// from the nondet input, as ESBMC prints it (#311).
// EXPECT: ^  t\.b = 25;
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
struct T {
  int a;
  int b;
};

int main(void) {
  int k = __VERIFIER_nondet_int();
  struct T t;
  t.a = 1;
  t.b = k * k;
  __VERIFIER_assert(t.a + t.b != 26);
  return 0;
}
