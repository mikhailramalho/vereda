int analyze_this()
{
  char *p_char=malloc(sizeof(char));
  int *p_int=malloc(sizeof(int));
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
// Imported from ESBMC regression/esbmc/00_memory_leak_02
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
