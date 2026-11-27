// RUN: --property-file %S/properties/no-overflow.prp
// EXPECT: ^  arithmetic overflow on add$
// EXPECT: ^FALSE_OVERFLOW$
// EXPECT-NOT: division by zero
// Under no-overflow a division by zero is no property and no assumption
// either: the path with x == 0 reaches the overflow.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = 10 / x;
  if (x == 0) {
    int z = __VERIFIER_nondet_int();
    if (z != 2147483647)
      return 0;
    return z + 1;
  }
  return y;
}
