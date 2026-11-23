/* #9060: an aggregate local leaves no member entries in the value set once
 * it is dead. */
struct S
{
  int *p;
  int *q;
};
int g, h;

void f(void)
{
  struct S s = {&g, &h};
  *s.p = 1;
}

int main(void)
{
  {
    struct S t = {&g, &h};
    *t.q = 2;
  }
  f();
  int *x = &g;
  __VERIFIER_assert(*x == 1); // f wrote g
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9060
// Needs: --show-symex-value-sets
// RUN: --show-symex-value-sets
// MODE: FUTURE
// EXPECT: ^        ASSIGN x=&g;$
// EXPECT: \A(?![\s\S]*ASSIGN x=&g;\n\n\n(?:[^\n]+\n)*[^\n]*@t\?1!0\.[pq] = )
// EXPECT: \A(?![\s\S]*ASSIGN x=&g;\n\n\n(?:[^\n]+\n)*[^\n]*@F@f@s\?1!0\.[pq] = )
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
