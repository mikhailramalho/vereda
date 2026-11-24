// #7964: p leaves its loop-entry object; the inductive step must not assume it stays.
int a[1];
int b[5];
int main()
{
  int *p = a;
  int s = 0;
  while (1)
  {
    if (__ESBMC_same_object(p, b))
    {
      s += *p; // reads b[5] on the seventh iteration
      p++;
    }
    else
      p = b;
  }
}
// Imported from ESBMC regression/k-induction/github_7964_leave_entry
// Needs: __ESBMC_same_object
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s+FAILED\s+\[main\.array-bounds-violated\.1\]\s+line 12\s+dereference failure: array bounds violated$
