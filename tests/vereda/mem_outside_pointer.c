// Pointers from outside the program (#137) point to no object whose address
// the program keeps to itself: a call without a body returns one, main gets
// argv, an address is made from an integer, a pointer is read from memory
// the outside filled. A store through any of them leaves the counter alone.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
extern void __VERIFIER_assert(int);
extern unsigned long __VERIFIER_nondet_ulong(void);

struct regs { int ctrl; };
struct dev { struct regs *regs; };
extern struct dev *get_dev(void);
extern int *ioremap(unsigned long, unsigned long);

int counter;
int table[4];

int main(int argc, char **argv) {
  int *own = &table[1];
  uintptr_t a = (uintptr_t)own; // table's address goes out, the counter's does not
  if (argc > 1)
    argv[1][0] = 'x';
  int *mmio = ioremap(0xfee00000, 4);
  if (mmio)
    *mmio = 1;
  struct dev *d = get_dev();
  if (d && d->regs)
    d->regs->ctrl = 2;
  *(int *)0x1000 = 3;
  *(int *)__VERIFIER_nondet_ulong() = 4;
  *(int *)a = 5;
  __VERIFIER_assert(counter == 0);
  return 0;
}
