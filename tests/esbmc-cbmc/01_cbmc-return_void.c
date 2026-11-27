void f()
{
}

void g()
{
  return f();
}

int main()
{
  g();
}
// Imported from ESBMC regression/cbmc/01_cbmc-return_void
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
