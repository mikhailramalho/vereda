// Constant switch inside a loop exercises the constant SwitchInst fast path
// for both explicit cases and the default destination.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: VERIFICATION FAILED
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
  // i = 0..11 -> mod 5: 0,1,2,3,4, 0,1,2,3,4, 0,1 -> 2*(1+2+3+4+10) + 1 + 2 = 43
  __VERIFIER_assert(acc == 43);
  return 0;
}
