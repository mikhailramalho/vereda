// Any other asm gives its register outputs nondet values and, without a
// "memory" clobber, writes no memory, even memory it reads.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);

int main(void) {
  int a = 7, y, z;
  __asm__ __volatile__("movl %2, %0\n\tincl %1" : "=r"(y), "=r"(z) : "m"(a), "1"(a));
  __VERIFIER_assert(a == 7);
  return 0;
}
