// The kernel's MMIO accessors (asm/io.h): writel stores, and readl, a mov
// into a register, writes no memory despite its "memory" clobber.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

static unsigned readl(const volatile void *addr) {
  unsigned ret;
  __asm__ __volatile__("movl %1,%0" : "=r"(ret) : "m"(*(volatile unsigned *)addr) : "memory");
  return ret;
}
static void writel(unsigned val, volatile void *addr) {
  __asm__ __volatile__("movl %0,%1" : : "r"(val), "m"(*(volatile unsigned *)addr) : "memory");
}
static void writeb(unsigned char val, volatile void *addr) {
  __asm__ __volatile__("movb %0,%1" : : "q"(val), "m"(*(volatile unsigned char *)addr) : "memory");
}

int main(void) {
  unsigned reg[2] = {0, 0}, v = __VERIFIER_nondet_uint();
  writel(v, &reg[1]);
  writeb(0x5a, &reg[0]);
  (void)readl(&reg[1]);
  __VERIFIER_assert(reg[0] == 0x5a && reg[1] == v);
  return 0;
}
