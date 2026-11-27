struct inner
{
  int n;
  int m;
};

struct outer
{
  struct inner in;
  int k;
};

int main(void)
{
  struct outer x;
  x.in.n = 4;
  x.in.m = 7;
  x.k = 9;

  int s = 0;
  for (int i = 0; i < x.in.n; i++)
    s++;

  __VERIFIER_assert(s == 4); // the loop runs x.in.n times
  __VERIFIER_assert(x.in.m == 7 && x.k == 9); // the sibling members keep their values
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_member_loop_bound
// EXPECT: ^VERIFICATION SUCCESSFUL$
