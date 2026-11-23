int a[3];

int main()
{
  int i = 0;
  int s = 0;
  goto test;
body:
  if (a[i] != 0)
    s = s + a[i];
  i = i + 1;
test:
  if (i <= 2)
    goto body;
  return s;
}
// Imported from ESBMC regression/termination/github_7565
// Needs: --termination --max-inductive-step 3
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^.*all executions terminate.*$
