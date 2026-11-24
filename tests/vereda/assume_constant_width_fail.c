// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  SUCCESS +\[main\.assertion\.1\] +line 12 
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 13 
// An assumption on x's low byte says nothing about its upper bits: the
// constant replaces the truncated value only, never x (#120).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume((signed char)x == -1);
  __VERIFIER_assert((unsigned char)x == 255);
  __VERIFIER_assert(x == -1);
  return 0;
}
