// --incremental-bmc gives up at --max-k-step without a verdict.
// RUN: --incremental-bmc --max-k-step 3
// EXPECT: ^\[INFO\] Bound 3 too small: unwinding assertion loop 0$
// EXPECT: ^VERIFICATION UNKNOWN: no verdict up to bound 3$
extern int __VERIFIER_nondet_int(void);

int main(void) {
  while (__VERIFIER_nondet_int())
    ;
  return 0;
}
