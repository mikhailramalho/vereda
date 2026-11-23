// A null pointer stored on one path reads back as null after the join. The
// read goes through the memory arrays, which rebuild a pointer from its
// object's base, so the null object's base must be 0 (issues #6, #7, #8).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
struct node { int a; struct node *next; };
int main(void) {
  struct node *l = 0;
  if (__VERIFIER_nondet_int()) {
    struct node *p = malloc(sizeof(*p));
    if (!p)
      abort();
    p->a = 0;
    p->next = 0;
    l = p;
  }
  if (l)
    __VERIFIER_assert(l->next == 0);
  return 0;
}
