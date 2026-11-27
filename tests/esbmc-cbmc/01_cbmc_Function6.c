int global;

void f()
{
  void g();
  
  g();
}

void g()
{
  global=123;
}

int main()
{
  f();
  assert(global==123);
  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function6
// EXPECT: ^VERIFICATION SUCCESSFUL$
