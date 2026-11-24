// test_and_set_bit gives the bit it found, so a second call on the same bit
// finds it set.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

#define LOCK_PREFIX ".section .smp_locks,\"a\"\n.balign 4\n.long 671f - .\n.previous\n671:\n\tlock; "

static int test_and_set_bit(int nr, volatile unsigned long *addr) {
  int oldbit;
  __asm__ __volatile__(LOCK_PREFIX "bts %2,%1\n\tsbb %0,%0"
                       : "=r"(oldbit), "+m"(*(volatile long *)addr)
                       : "Ir"(nr)
                       : "memory");
  return oldbit;
}

int main(void) {
  unsigned long bits = 0;
  int nr = __VERIFIER_nondet_int() & 63;
  test_and_set_bit(nr, &bits);
  __VERIFIER_assert(test_and_set_bit(nr, &bits) == 0);
  return 0;
}
