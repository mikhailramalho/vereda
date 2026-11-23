// #2240: a call through a function pointer that may hold a data object's
// address aborted --show-goto-value-sets.
int plugins;
int x;

int run(int *q)
{
  return *q;
}

int main()
{
  int c;
  int (*code_ptr)(int *) = run;
  if (c)
    code_ptr = (int (*)(int *))&plugins;
  return code_ptr(&x);
}
// Imported from ESBMC regression/esbmc/github_2240
// Needs: --show-goto-value-sets
// RUN: --show-goto-value-sets
// MODE: FUTURE
// EXPECT: @main@code_ptr = \{ .*<plugins, 0, 1, signed int>
// EXPECT: @run@q = \{ <x, 0, 1, signed int> \}
// Vereda: verdict none
