// An asm that runs nothing inline, or only a fence or pause, does nothing in
// one thread, with or without a "memory" clobber; an empty asm whose output
// is tied to an input gives the input back (the kernel's OPTIMIZER_HIDE_VAR
// and RELOC_HIDE, which takes a pointer as an integer).
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int x = __VERIFIER_nondet_int(), y, z = x;
  __asm__ __volatile__("" ::: "memory");
  __asm__ __volatile__("mfence" ::: "memory");
  __asm__ __volatile__("lfence" ::: "memory");
  __asm__ __volatile__("sfence" ::: "memory");
  __asm__ __volatile__("rep; nop" ::: "memory");
  __asm__ __volatile__(".pushsection .discard\n.long 0\n.popsection\n1:" : "=m"(z));
  __asm__("" : "=r"(y) : "0"(x));
  unsigned long p;
  __asm__("" : "=r"(p) : "0"(&x));
  __VERIFIER_assert(y == x && z == x && p == (unsigned long)&x);
  return 0;
}
