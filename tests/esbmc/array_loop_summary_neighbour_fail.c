int a[50];
int main()
{
  a[0] = 1;
  for (int i = 1; i < 50; i++)
    a[i] = a[i - 1] + 1;
  __VERIFIER_assert(a[49] == 49); // a[49] == 49
}
// Imported from ESBMC regression/esbmc/array_loop_summary_neighbour_fail
// RUN: --unwind 51 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
