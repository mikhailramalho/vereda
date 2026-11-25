// With an SV-COMP property file, vereda.py checks the small bounds one at a
// time before stepping by 2: this bug needs bound 2, which a step of 2 from
// bound 1 skips for bound 3, where some tasks' solve is far slower (#174).
// RUN: --incremental-bmc --property-file %S/properties/unreach-call.prp
// EXPECT: Checking bound k = 2$
// EXPECT-NOT: Checking bound k = 3$
// EXPECT: ^FALSE_REACH$
extern void reach_error(void);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int i = 0;
  while (__VERIFIER_nondet_int())
    i++;
  if (i == 1)
    reach_error();
  return 0;
}
