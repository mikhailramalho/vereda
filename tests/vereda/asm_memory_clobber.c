// A "memory" clobber on an asm that runs something lets it write any memory:
// unsupported when reached, but not where no path reaches it.
// RUN: -m64
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: inline asm that may write memory at
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

static int g;

void touch(void) { __asm__ __volatile__("movl $1, g(%%rip)" ::: "memory"); }

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x != x)
    touch();
  __VERIFIER_assert(g == 0);
  touch();
  __VERIFIER_assert(g == 0);
  return 0;
}
