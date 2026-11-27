int f()
{
  return 1;
}

int main(int argc, char* p[])
{
  int i;
  int x = ({f();});
  assert(x==1);

  int y = ({ i = f(); });
  assert(y==1);
  assert(i==1);

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Statement_Expression2
// EXPECT: ^VERIFICATION SUCCESSFUL$
