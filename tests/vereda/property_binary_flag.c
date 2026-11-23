// RUN: --property unreach-call
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^FALSE_REACH$
// The binary's --property passes through vereda.py, which must not read it
// as an abbreviation of its own --property-file.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  if (__VERIFIER_nondet_int() == 5)
    reach_error();
  return 0;
}
