// RUN: --property-file %S/properties/no-overflow.prp
// EXPECT: ^  arithmetic overflow on add$
// EXPECT: ^FALSE_OVERFLOW$
// EXPECT-NOT: reach_error$
// Under no-overflow clang's sanitizer checks are the properties; a reachable
// reach_error is not one.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 0)
    reach_error();
  return x + 1;
}
