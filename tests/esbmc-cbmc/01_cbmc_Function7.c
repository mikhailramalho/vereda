typedef unsigned B[8];

void fun(B b)
{
}

int main(void)
{
  B var;
  unsigned *p = &var;
  fun(p);
}
      
// Imported from ESBMC regression/cbmc/01_cbmc_Function7
// EXPECT: ^VERIFICATION SUCCESSFUL$
