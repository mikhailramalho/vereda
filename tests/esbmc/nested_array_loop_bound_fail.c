int main(void)
{
  int a[2][2];
  a[1][1] = 4;

  int s = 0;
  for (int i = 0; i < a[1][1]; i++)
    s++;

  __VERIFIER_assert(s == 5); // the loop runs one time too many
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_loop_bound_fail
// EXPECT: ^VERIFICATION FAILED$
