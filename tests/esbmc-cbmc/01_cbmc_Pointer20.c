int *p;
int global;

void f() {
  int local;
  int input;
  
  input=__VERIFIER_nondet_int();

  p=input?&local:&global;
}

int main() {
  int z;

  f();

  z=*p;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer20
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
