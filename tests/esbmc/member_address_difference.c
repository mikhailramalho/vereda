struct P
{
  int a;
  int b;
};

int main()
{
  struct P p;
  unsigned n = (unsigned)(&p.b - &p.a);
  int s = 0;
  for (unsigned i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s == 1); // one int between the two members
  return 0;
}
// Imported from ESBMC regression/esbmc/member_address_difference
// RUN: --unwind 4
// EXPECT: ^VERIFICATION SUCCESSFUL$
