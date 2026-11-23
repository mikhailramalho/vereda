int main()
{
  int i = 0;
  int c = 1;
  goto test;
body:
  i = i + 1;
test:
  if (c == 1)
    goto body;
  return 0;
}
// Imported from ESBMC regression/termination/github_7565_fail
// Needs: --termination --max-inductive-step 3
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*non-terminating execution.*$
// Vereda: verdict none
