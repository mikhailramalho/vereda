int main()
{
  float f;
  double d;
  unsigned char x;

  d=f;
  
  if(f==x)
    assert(d==x);
}
// Imported from ESBMC regression/floats/Float12
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
