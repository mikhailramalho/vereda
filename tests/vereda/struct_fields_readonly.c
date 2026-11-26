// RUN: --no-assertions
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
struct entry { unsigned char x; unsigned short y; };
static const struct entry a[256] = {0};
int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 256);
  ((struct entry *)a)[i].y = 42;
}
