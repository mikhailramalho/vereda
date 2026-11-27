typedef struct {
 int i;
} xt;

xt *x, **xx;

int main()
{
  xt y;

  x=&y;

  xx=&x;

  x->i=__VERIFIER_nondet_int();
  
  assert(x->i==2);

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function_Pointer13
// EXPECT: ^VERIFICATION FAILED$
