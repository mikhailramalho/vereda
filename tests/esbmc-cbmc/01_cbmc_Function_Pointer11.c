unsigned fritz(unsigned x) { return x+1; }
unsigned franz(unsigned x) { return x; }

typedef unsigned (fun_t)(unsigned);

fun_t *f;

int main(int argc, char **argv)
{
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x>20);
  if(x<10)
    f=0;
  else
    f=franz;

  // should be fine, and pass without warning
  f(x);
} 
// Imported from ESBMC regression/cbmc/01_cbmc_Function_Pointer11
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
