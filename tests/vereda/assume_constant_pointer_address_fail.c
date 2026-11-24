// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  SUCCESS +\[main\.assertion\.1\] +line 13 
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 15 
// An assumption on p's address makes that integer a constant, not p:
// either object may sit at that address, so p may still name b (#120).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int a[1], b[1];
int main(void) {
  int *p = __VERIFIER_nondet_int() ? &a[0] : &b[0];
  __VERIFIER_assume((unsigned long)p == 0x1000);
  __VERIFIER_assert((unsigned long)p + 4 == 0x1004);
  *p = 1;
  __VERIFIER_assert(a[0] == 1);
  return 0;
}
