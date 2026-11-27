int f(void) { return 0; }
int main()
{
  *(int *)(void *)f = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/function_write_plain_fail
// EXPECT: ^  dereference failure: Program code accessed in write or free mode$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: Program code accessed in write or free mode$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
