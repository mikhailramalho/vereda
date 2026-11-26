// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct entry { unsigned short x, y; };
static void escape(struct entry *p) { p[1].y = 42; }
int main(void) {
  struct entry a[16] = {0};
  escape(a);
  // A byte write overlapping a word access requires the byte model.
  ((unsigned char *)&a[1].y)[0] = 17;
  __VERIFIER_assert(a[1].y == 17);
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 16);
  __VERIFIER_assert(a[i].x == 0);
  struct entry b[16] = {0};
  b[1].y = 42;
  ((unsigned char *)&b[1].y)[0] = 17;
  __VERIFIER_assert(b[i].y == (i == 1 ? 17 : 0));
}
