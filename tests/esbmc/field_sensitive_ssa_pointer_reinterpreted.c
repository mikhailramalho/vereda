#include <assert.h>

struct S
{
  int a;
  int b;
};

int main(void)
{
  struct S s, u;
  s.a = 0;
  s.b = 0;
  struct S *p = &s;
  struct S *q = &u;
  q->a = 1;
  char *c = (char *)p;
  c[0] = 1;
  assert(s.a == 1 && u.a == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_pointer_reinterpreted
// MODE: FUTURE
// EXPECT: ^Field-sensitive SSA splits 1 struct object\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL
