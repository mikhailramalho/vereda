void sort(int *ptr)
{
  if (ptr[1] == ptr[2])
    ;
}

int main()
{
  int x = 1;
  int *y = &x;
  sort(y);
}
// Imported from ESBMC regression/goto-coverage/github_2124_3
// Needs: --no-standard-checks --branch-coverage
// RUN: --function sort --no-standard-checks --branch-coverage
// MODE: FUTURE
// EXPECT: ^Branches : 2
// EXPECT: ^Reached : 2
// EXPECT: ^Branch Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
