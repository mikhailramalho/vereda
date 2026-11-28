// RUN: --overflow-check
// EXPECT: ^  FAILED       \[main\.arithmetic-overflow\.1\]  line 12  arithmetic overflow on add$
// EXPECT-NOT: arithmetic overflow on sub
// EXPECT: ^VERIFICATION FAILED$
// Without --multi-property a failed overflow check still ends its path:
// b - 1 overflows only past a + 1's failed check, so it is never reported.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
int main(void) {
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a > 2147483646);
  int b = a + 1;
  int c = b - 1;
  return c == 0;
}
