struct S
{
  int a, b;
};

int main()
{
  struct S s[10];
  int *p, *q, x=__VERIFIER_nondet_int(), y=__VERIFIER_nondet_int();
  
  __VERIFIER_assume(x==0);
  __VERIFIER_assume(y==0);
  
  p=&(s[x].a);
  q=&(s[y].a);
  
  assert(p==q);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer11
// EXPECT: ^VERIFICATION SUCCESSFUL$
