// `*a` on an array is `a[0]`. Left as a dereference the IREP2 adjust pass
// handed the encoder a pointer built from an array and it aborted there.
int main(void)
{
  int a[3] = {0, 0, 0};

  *a = 7;

  __VERIFIER_assert(a[0] == 7); // *a assigns the first element
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_deref_array
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
