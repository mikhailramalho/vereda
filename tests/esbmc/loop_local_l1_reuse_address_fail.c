int main()
{
  int *p = 0;
  for (int i = 0; i < 2; i++)
  {
    int x = i;
    if (i == 0)
      p = &x;
    else
      return *p;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/loop_local_l1_reuse_address_fail
// EXPECT: FAILED +\[main\.expired-pointer-dereference\.1\]
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: FAILED +\[main\.expired-pointer-dereference\.1\]
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
