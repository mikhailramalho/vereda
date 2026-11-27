struct my_struct
{
  int i;
  int array[10];
} s;

void f(int *p)
{
  int diff;
  
  diff=p-&s.array[0];

  // should pass
  *p=1;
}

int main()
{
  int ind, x;
  ind=x;
  int *p=&s.array[ind];
  
  if(ind>=0 && ind<=9)
    f(p);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Structs7
// Needs: --no-pointer-arith-check
// RUN: --no-pointer-arith-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
