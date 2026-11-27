int main()
{
  double a;
  __VERIFIER_assume(a==a);
  double b = a;

  union {
    double f;
    long long unsigned int i; // needs to have 64 bits
  } au, bu;
  
  au.f = a;
  bu.f = b;

  assert(au.i == bu.i);
  assert(a == b);
}
// Imported from ESBMC regression/floats/Float-no-simp5
// EXPECT: ^VERIFICATION SUCCESSFUL$
