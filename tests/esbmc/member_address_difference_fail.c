struct P
{
  int a;
  int b;
};

int main()
{
  struct P p;
  unsigned n = (unsigned)((char *)&p.b - (char *)&p.a);
  int s = 0;
  for (unsigned i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s == 9); // the members are not nine bytes apart
  return 0;
}
// Imported from ESBMC regression/esbmc/member_address_difference_fail
// RUN: --unwind 6
// EXPECT: ^VERIFICATION FAILED$
