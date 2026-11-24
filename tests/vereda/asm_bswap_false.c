// bswap moves the low byte, so it is not kept for every value.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned v = __VERIFIER_nondet_uint(), r;
  __asm__("bswapl %0" : "=r"(r) : "0"(v));
  volatile unsigned rv = r;
  __VERIFIER_assert((rv & 0xff) == (v & 0xff));
  return 0;
}
