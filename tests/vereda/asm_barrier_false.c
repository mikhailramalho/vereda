// A compiler barrier lets the path go on, so a violation after it is found.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int x = __VERIFIER_nondet_int(), y;
  __asm__ __volatile__("" ::: "memory");
  __asm__("" : "=r"(y) : "0"(x));
  __VERIFIER_assert(y != 5);
  return 0;
}
