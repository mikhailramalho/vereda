int c(void);
int main()
{
  int *p = 0;
  while (1)
  {
    int a = 1;
    p = &a;
    while (c())
      a++;
    break;
  }
  return *p;
}
// Imported from ESBMC regression/esbmc/break_after_inner_loop_fail
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: accessed expired variable pointer `a'
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: accessed expired variable pointer `a'
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
