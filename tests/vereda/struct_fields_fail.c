// EXPECT: ^VERIFICATION FAILED$
// EXPECT: main.assertion.1
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct entry { unsigned char x; unsigned short y; };
static struct entry a[256];
int main(void) {
  for (unsigned i = 0; i < 256; ++i)
    a[i].y = 3 * i;
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 256);
  __VERIFIER_assert(a[i].y != 3 * i);
}
