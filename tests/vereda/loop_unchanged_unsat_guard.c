// EXPECT: ^VERIFICATION SUCCESSFUL$
// A deferred feasibility query must still stop an unreachable long loop.
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
volatile unsigned sink;
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x * x == 2u); // no square is 2 modulo 4
  for (unsigned i = 0; i < 1000000; ++i)
    sink = i;
  __VERIFIER_assert(0);
}
