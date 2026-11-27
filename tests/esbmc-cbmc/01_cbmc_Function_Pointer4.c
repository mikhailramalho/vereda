struct device {
  int (*func)();
};

int one()
{
  return 1;
}

int main(void)
{
  struct device devices[1];
  int x;
  
  devices[0].func = one;
  
  x=(* devices[0].func)();
  assert(x == 1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function_Pointer4
// EXPECT: ^VERIFICATION SUCCESSFUL$
