// The quiet half of the same defect. `*"abc"` dereferences an array without
// reaching the encoder's int/ptr cast, so the unported arm did not abort here
// -- it returned a wrong verdict, which a crash-class test cannot catch.
int main(void)
{
  char c = *"abc";

  __VERIFIER_assert(c == 'a'); // *\"abc\" is the first character
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_deref_array_strlit
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
