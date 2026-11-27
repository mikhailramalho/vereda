int g, k;

int main()
{
  int r1, r2;
  
  r1= (g++) ? : 2;
  
  assert(r1==2);
  assert(g==1);

  r2= (g++) ? : (k++);
  
  assert(r2==1);
  assert(g==2);  
  assert(k==0);
}
// Imported from ESBMC regression/cbmc/01_cbmc_gcc_conditional_expr1
// EXPECT: ^VERIFICATION SUCCESSFUL$
