// incl on a long changes its low four bytes only, which is not an add to the
// long, so it is no kernel atomic: unsupported, as it writes memory.
// RUN: -m64
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: inline asm that may write memory at
extern void __VERIFIER_assert(int);

int main(void) {
  long v = 0xffffffffL;
  __asm__ __volatile__("lock; incl %0" : "+m"(v));
  __VERIFIER_assert(v == 0);
  return 0;
}
