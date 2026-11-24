// What an asm puts in a register output is not interpreted: it may be
// anything, so even this holding assertion is reported.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);

int main(void) {
  int y;
  __asm__("movl $7, %0" : "=r"(y));
  __VERIFIER_assert(y == 7);
  return 0;
}
