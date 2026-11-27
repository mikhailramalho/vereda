/* A variable-length array as a struct member is a GCC extension that clang
   declines to implement -- its own diagnostic says the extension "will never
   be supported" -- so ESBMC, whose frontend is clang, cannot parse it either.

   That is a limitation rather than a defect, but the failure mode still
   matters: it must stay a clean parse diagnostic and never become a crash.
   This pins that (issue #4086). */
void f(int n)
{
  struct S
  {
    int arr[n];
  };
  struct S s;
  s.arr[0] = 42;
}

int main(void)
{
  f(3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4086_vla_in_struct
// RUN: --no-incremental-bmc
// EXPECT: ^.*fields must have a constant size: 'variable length array in structure'
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
