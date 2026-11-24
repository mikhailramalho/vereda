// An asm that may write memory, through a memory output or a "memory"
// clobber, is unsupported when reached.
// RUN: -m64
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: inline asm that may write memory at
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int x = 0;
  if (__VERIFIER_nondet_int())
    __asm__ __volatile__("movl $1, %%eax\n\tmovl %%eax, %0" : "=m"(x) : : "eax");
  __VERIFIER_assert(x == 0);
  return 0;
}
