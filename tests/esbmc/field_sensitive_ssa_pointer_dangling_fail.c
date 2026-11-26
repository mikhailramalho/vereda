struct S
{
  int a;
  int b;
};

struct S *g;

void f(void)
{
  struct S s;
  s.a = 1;
  s.b = 2;
  g = &s;
}

int main(void)
{
  f();
  return g->a;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_pointer_dangling_fail
// RUN:
// MODE: FUTURE
// EXPECT: ^Field-sensitive SSA splits 1 struct object\(s\)$
// EXPECT: ^  dereference failure: accessed expired variable pointer `s'$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL
