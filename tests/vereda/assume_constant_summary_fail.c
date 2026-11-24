// RUN: --multi-property
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 21
// EXPECT: ^VERIFICATION FAILED$
// A summarised loop walks one iteration, where only n == 5 goes on past
// the check; the loop may run no iteration at all, so n stays open after
// it (#120).
extern void __VERIFIER_assert(int);
extern void reach_error(void);
extern unsigned __VERIFIER_nondet_uint(void);
unsigned char a[100];
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint();
  if (m > 100)
    return 0;
  for (unsigned i = 0; i < m; i++) {
    if (n != 5)
      reach_error();
    a[i] = 1;
  }
  __VERIFIER_assert(n == 5);
  return 0;
}
