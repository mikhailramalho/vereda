/* Copy new sign bit to the left */
int test_extend2(int x, unsigned width)
{
  int mask = 1 << (width-1);
  int bit = (x & mask) << 1;
  mask <<= 1;
  while (mask) {
    x = bit | (~mask & x);
    mask <<= 1;
    bit <<= 1;
  } 
  return x;
}

int main()
{
  int r;
  
  r=test_extend2(4, 3);
  assert(r==-4);
}
// Imported from ESBMC regression/esbmc/github_578_fail5
// RUN: --unwind 32 --overflow-check
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
