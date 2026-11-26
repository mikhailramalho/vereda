#include <assert.h>
struct in { int x; int y; };
struct out { struct in a; struct in b; };
void f(struct out *p) { p->a.y = 5; p->b = p->a; }
int main(void)
{
  struct out o = {{1,2},{3,4}};
  f(&o);
  assert(o.b.y == 5);
  assert(o.b.x == 1);
  assert(o.a.x == 2); // fails
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_leaf_nested_ptr_fail
// RUN: --unwind 3
// EXPECT: ^Field-sensitive SSA splits 3 struct object\(s\)$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^Field-sensitive SSA splits 3 struct object\(s\)$
// MODE: FUTURE
// Vereda: verdict FAILED
