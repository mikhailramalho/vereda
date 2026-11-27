struct s
{
  int *p;
};

struct s array[2];

int main()
{
  int z, k, *q;

  array[0].p=&k;
  
  q=array[0].p;
  
  z=*q;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer6
// EXPECT: ^VERIFICATION SUCCESSFUL$
