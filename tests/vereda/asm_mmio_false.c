// readl gives what the device register holds, which need not be what was
// written to it: any value.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);

static unsigned readl(const volatile void *addr) {
  unsigned ret;
  __asm__ __volatile__("movl %1,%0" : "=r"(ret) : "m"(*(volatile unsigned *)addr) : "memory");
  return ret;
}
static void writel(unsigned val, volatile void *addr) {
  __asm__ __volatile__("movl %0,%1" : : "r"(val), "m"(*(volatile unsigned *)addr) : "memory");
}

int main(void) {
  unsigned reg = 0;
  writel(5, &reg);
  __VERIFIER_assert(readl(&reg) == 5);
  return 0;
}
