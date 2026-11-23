struct inner
{
  int n;
};

struct outer
{
  struct inner in;
};

int main(void)
{
  struct outer x;
  x.in.n = 4;
  x.in.n = 6;

  int s = 0;
  for (int i = 0; i < x.in.n; i++)
    s++;

  __VERIFIER_assert(s == 6); // the loop reads the second write, not the first
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_member_loop_bound_overwrite
// EXPECT: ^VERIFICATION SUCCESSFUL$
