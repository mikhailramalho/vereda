// RUN: --overflow-check --multi-property
// EXPECT: ^  FAILED       \[main\.arithmetic-overflow\.1\]  line 13  arithmetic overflow on add$
// EXPECT: ^  FAILED       \[main\.arithmetic-overflow\.2\]  line 14  arithmetic overflow on sub$
// EXPECT: ^VERIFICATION FAILED$
// Under --multi-property a run goes on past a failed overflow check, with
// the wrapped result: a + 1 always overflows to INT_MIN, and b - 1 then
// overflows too, which only a path past the first check can reach.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
int main(void) {
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a > 2147483646);
  int b = a + 1;
  int c = b - 1;
  return c == 0;
}
