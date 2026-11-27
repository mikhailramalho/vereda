// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^TRUE$
// The control for property_unreach_div_zero.c: x == 0 is excluded before the
// division, so reach_error is unreachable.
extern void abort(void);
extern void reach_error(void);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 0)
    return 0;
  int y = 10 / x;
  if (x == 0) {
    reach_error();
    abort();
  }
  return y;
}
