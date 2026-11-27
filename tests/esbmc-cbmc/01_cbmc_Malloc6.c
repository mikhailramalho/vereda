int analyze_this()
{
  char *p_char=malloc(sizeof(char));
  __VERIFIER_assume(p_char);
  int *p_int=malloc(sizeof(int));
  __VERIFIER_assume(p_int);
  void *p;
  
  p=__VERIFIER_nondet_bool()?p_char:p_int;

  p_int=p;
  
  // this should not fail
  if((void *)p_int!=(void *)p_char)
    *p_int=1;
}

int main()
{
  analyze_this();
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc6
// EXPECT: ^VERIFICATION SUCCESSFUL$
