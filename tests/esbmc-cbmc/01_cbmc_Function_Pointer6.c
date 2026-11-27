
struct S {
  int (* func) ();
};

int ten()
{
  return 10;
}

int twenty()
{
  return 20;
}

int main(void)
{
  int (*ppp)();
  struct S * ps = (struct S *) malloc(sizeof(struct S));
  __VERIFIER_assume(ps);
  int x, y;

  ps->func = x?ten:twenty;

  ppp=ps->func;
  
  y=ps->func();
  
  assert(y==10 || y==20);

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function_Pointer6
// EXPECT: ^VERIFICATION SUCCESSFUL$
