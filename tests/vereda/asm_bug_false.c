// ud2 ends the path only where the program expects no return: newer
// kernels' WARN() runs ud2 and resumes after it, so here the path goes on.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 3)
    __asm__ __volatile__("ud2");
  __VERIFIER_assert(x != 3);
  return 0;
}
