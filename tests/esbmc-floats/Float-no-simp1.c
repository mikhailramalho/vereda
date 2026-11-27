int main()
{
  unsigned int i, j;
  double d;
  
  i=100.0;
  d=i;
  j=d;
  assert(j==100);
}
// Imported from ESBMC regression/floats/Float-no-simp1
// EXPECT: ^VERIFICATION SUCCESSFUL$
