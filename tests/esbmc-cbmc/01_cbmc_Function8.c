// this exposes a problem with the renaming of "ignore"
// due to the inlining

inline void baz(unsigned short ignore)
{
  // should fail
  __VERIFIER_assert(0); // KABOOM
}

static void foo()
{
    baz(1);
}

static void bar()
{
    baz(0);
}

int main()
{
  foo();

  bar();       

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function8
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
