int f(int n)
{
  int buf[8];
  buf[0] = n;
  if (n <= 0)
    return buf[0];
  return f(n - 1) + buf[0];
}

int main(void)
{
  return f(3);
}
// Imported from ESBMC regression/esbmc/40_total_stack_recursion_false
// Needs: --stack-limit 512 --total-stack-limit 1279
// RUN: --unwind 6 --stack-limit 512 --total-stack-limit 1279
// MODE: FUTURE
// EXPECT: Total stack limit property was violated when declaring buf
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
