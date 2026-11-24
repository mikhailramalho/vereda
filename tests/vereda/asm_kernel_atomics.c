// The atomics, compare-exchange and bit operations of the kernel's x86 headers (3.x and 4.x)
// read, change and write their memory operand exactly; the lock prefix and
// its .smp_locks entry change nothing in one thread.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdbool.h>
extern int __VERIFIER_nondet_int(void);
extern unsigned long __VERIFIER_nondet_ulong(void);
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);

#define LOCK_PREFIX ".section .smp_locks,\"a\"\n.balign 4\n.long 671f - .\n.previous\n671:\n\tlock; "
#define BITOP_ADDR(x) "+m"(*(volatile long *)(x))
typedef struct { int counter; } atomic_t;

static void atomic_inc(atomic_t *v) { __asm__ __volatile__(LOCK_PREFIX "incl %0" : "+m"(v->counter)); }
static void atomic_dec(atomic_t *v) { __asm__ __volatile__(LOCK_PREFIX "decl %0" : "+m"(v->counter)); }
static void atomic_add(int i, atomic_t *v) {
  __asm__ __volatile__(LOCK_PREFIX "addl %1,%0" : "+m"(v->counter) : "ir"(i));
}
static void atomic_sub(int i, atomic_t *v) {
  __asm__ __volatile__(LOCK_PREFIX "subl %1,%0" : "+m"(v->counter) : "ir"(i));
}
static int atomic_dec_and_test(atomic_t *v) {
  unsigned char c;
  __asm__ __volatile__(LOCK_PREFIX "decl %0; sete %1" : "+m"(v->counter), "=qm"(c) : : "memory");
  return c != 0;
}
static int atomic_sub_and_test(int i, atomic_t *v) {
  unsigned char c;
  __asm__ __volatile__(LOCK_PREFIX "subl %2,%0; sete %1"
                       : "+m"(v->counter), "=qm"(c)
                       : "ir"(i)
                       : "memory");
  return c;
}
static int atomic_add_return(int i, atomic_t *v) {
  int old = i;
  __asm__ __volatile__(LOCK_PREFIX "xaddl %0, %1" : "+r"(i), "+m"(v->counter) : : "memory");
  return i + old;
}
static unsigned long xchg(unsigned long *p, unsigned long v) {
  __asm__ __volatile__("xchgq %0,%1" : "=r"(v), "+m"(*p) : "0"(v) : "memory");
  return v;
}
static int cmpxchg(int *p, int old, int new) {
  int ret;
  __asm__ __volatile__(LOCK_PREFIX "cmpxchgl %2,%1"
                       : "=a"(ret), "+m"(*p)
                       : "r"(new), "0"(old)
                       : "memory");
  return ret;
}
static void set_bit(unsigned int nr, volatile unsigned long *addr) {
  __asm__ __volatile__(LOCK_PREFIX "bts %1,%0" : BITOP_ADDR(addr) : "Ir"(nr) : "memory");
}
static void clear_bit(int nr, volatile unsigned long *addr) {
  __asm__ __volatile__(LOCK_PREFIX "btr %1,%0" : BITOP_ADDR(addr) : "Ir"(nr));
}
static int test_and_set_bit(int nr, volatile unsigned long *addr) {
  int oldbit;
  __asm__ __volatile__(LOCK_PREFIX "bts %2,%1\n\tsbb %0,%0"
                       : "=r"(oldbit), BITOP_ADDR(addr)
                       : "Ir"(nr)
                       : "memory");
  return oldbit;
}
static bool test_and_clear_bit(long nr, volatile unsigned long *addr) { // 4.x
  bool c;
  __asm__ __volatile__(LOCK_PREFIX "btr %2, %0; setc %1"
                       : "+m"(*addr), "=qm"(c)
                       : "Ir"(nr)
                       : "memory");
  return c;
}

int main(void) {
  int n = __VERIFIER_nondet_int();
  atomic_t a = {n};
  atomic_inc(&a);
  atomic_add(3, &a);
  atomic_sub(2, &a);
  atomic_dec(&a);
  __VERIFIER_assert(a.counter == n + 1);
  __VERIFIER_assert(atomic_add_return(4, &a) == n + 5 && a.counter == n + 5);
  __VERIFIER_assert(atomic_dec_and_test(&a) == (n == -4) && a.counter == n + 4);
  __VERIFIER_assert(atomic_sub_and_test(n + 4, &a) && a.counter == 0);

  unsigned long w = __VERIFIER_nondet_ulong(), w0 = w;
  __VERIFIER_assert(xchg(&w, 7) == w0 && w == 7);
  int c = n;
  __VERIFIER_assert(cmpxchg(&c, n, 3) == n && c == 3 && cmpxchg(&c, 4, 5) == 3 && c == 3);

  unsigned long bits[2] = {0, 0};
  int nr = __VERIFIER_nondet_int();
  __VERIFIER_assume(nr >= 0 && nr < 128);
  set_bit(nr, bits);
  __VERIFIER_assert(bits[nr / 64] == 1UL << (nr % 64) && bits[1 - nr / 64] == 0);
  __VERIFIER_assert(test_and_set_bit(nr, bits) == -1);
  __VERIFIER_assert(test_and_set_bit(3, &bits[1]) == (nr == 67 ? -1 : 0));
  __VERIFIER_assert(test_and_clear_bit(nr, bits) && !test_and_clear_bit(nr, bits));
  clear_bit(67, bits);
  __VERIFIER_assert(bits[0] == 0 && bits[1] == 0);
  return 0;
}
