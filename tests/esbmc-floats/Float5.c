int main()
{
  float a, b;
  
  __VERIFIER_assume(a==1 || a==0.5 || a==2 || a==3 || a==0.1);
  b=a;
  a/=2;
  a*=2;
  __VERIFIER_assert(a==b); // float div works
}
// Imported from ESBMC regression/floats/Float5
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
