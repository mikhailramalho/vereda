// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^FALSE_REACH$
// EXPECT-NOT: division by zero
// Under unreach-call a division by zero is no property and no assumption
// either (ESBMC's SV-COMP wrapper passes --no-div-by-zero-check): the path
// with x == 0 reaches reach_error.
extern void abort(void);
extern void reach_error(void);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = 10 / x;
  if (x == 0) {
    reach_error();
    abort();
  }
  return y;
}
