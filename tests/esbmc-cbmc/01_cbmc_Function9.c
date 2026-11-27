// this exposes a problem with the renaming of "ignore"
// due to the inlining

unsigned short g;

inline void baz()
{
  unsigned short ignore;
  ignore=g;
  // should fail
  __VERIFIER_assert(0); // KABOOM
}

static void foo()
{
    baz();
}

static void bar()
{
    baz();
}

int main()
{
  g=0;
  foo();

  g=1;
  bar();       

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function9
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
