int __VERIFIER_nondet_int();

double d = 0.0;

void f1()
{
  d = 1.0;
}

int main()
{
  int x = 2;

  if(__VERIFIER_nondet_int())
    x = 4;

  (void) f1();

  d += (x == 2);

  d += (x > 3);

  assert(d == 2.0);
}
// Imported from ESBMC regression/floats/Float3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
