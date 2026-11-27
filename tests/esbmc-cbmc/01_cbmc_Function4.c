int f1()
{
  int ret;
  return ret;
}

typedef struct {
  int x;
} Str1;

int main()
{
  Str1 st;
  int x;

  st.x = __VERIFIER_nondet_int();  // comment this line - and everything is OK
  st.x = f1();
  x = __VERIFIER_nondet_int();
  st.x = x;
  
  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function4
// EXPECT: ^VERIFICATION SUCCESSFUL$
