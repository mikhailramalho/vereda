// Constant switch inside a loop still catches a failing assertion after the
// loop.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: VERIFICATION SUCCESSFUL
extern void __VERIFIER_assert(int);

int main(void) {
  int acc = 0;
  for (int i = 0; i < 12; ++i) {
    switch (i % 5) {
    case 0: acc += 1; break;
    case 1: acc += 2; break;
    case 2: acc += 3; break;
    case 3: acc += 4; break;
    default: acc += 10; break;
    }
  }
  __VERIFIER_assert(acc == 42);
  return 0;
}
