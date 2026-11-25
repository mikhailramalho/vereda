// --k-step-from 3 checks bounds 1, 2 and 3, then steps by --k-step: this bug
// needs bound 4 and is found at 5 (#174).
// RUN: --incremental-bmc --unlimited-k-steps --k-step 2 --k-step-from 3
// EXPECT: Checking bound k = 2$
// EXPECT: Checking bound k = 3$
// EXPECT-NOT: Checking bound k = 4$
// EXPECT: Checking bound k = 5$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int i = 0;
  while (__VERIFIER_nondet_int())
    i++;
  __VERIFIER_assert(i != 3);
  return 0;
}
