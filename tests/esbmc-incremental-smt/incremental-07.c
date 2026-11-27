int main()
{
  unsigned int x, y = 0;
  for(x = 0; x < 1000; x++)
  {
    y = y + x;
    if(y > 10)
      assert(0);
  }
  return 0;
}
// Imported from ESBMC regression/incremental-smt/incremental-07
// EXPECT: ^VERIFICATION FAILED$
