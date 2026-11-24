// ud2, as the kernel's BUG() runs it, ends the path as __builtin_trap does:
// neither the loop CIL leaves after it nor __builtin_unreachable is reached.
// RUN: -m64 --unwind 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

#define BUG()                                                                                     \
  __asm__ __volatile__("1:\tud2\n.pushsection __bug_table,\"a\"\n2:\t.long 1b - 2b, %c0 - 2b\n"   \
                       "\t.word %c1, 0\n\t.org 2b+%c2\n.popsection"                               \
                       :                                                                          \
                       : "i"("bug.c"), "i"(10), "i"(12UL))

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 3) {
    BUG();
  stuck:
    goto stuck;
  }
  if (x == 4) {
    BUG();
    __builtin_unreachable();
  }
  __VERIFIER_assert(x != 3 && x != 4);
  return 0;
}
