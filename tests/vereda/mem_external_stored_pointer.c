// EXPECT: ^VERIFICATION SUCCESSFUL$
// A call without a body writes nothing through a stored null or through a
// pointer to a constant (#114), nor through a stored pointer when LLVM says
// it writes only its arguments' memory (frexp).
#include <math.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern int __VERIFIER_nondet_int(void);
struct S {
  int *p;
  const char *name;
};
void ext(struct S *);
static const char name[] = "abc";
// A read LLVM does not fold from the constant's initialiser.
static int at(const char *p, int i) { return p[i]; }
int main(void) {
  struct S s = {0, name};
  ext(&s);
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 3);
  __VERIFIER_assert(at(name, i) != 0);
  int x = 0;
  struct {
    int e;
    int *p;
  } f = {0, &x};
  frexp(1.0, &f.e);
  __VERIFIER_assert(x == 0);
  return 0;
}
