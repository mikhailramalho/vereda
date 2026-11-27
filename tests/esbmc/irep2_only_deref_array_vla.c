// Same rewrite on a variably-modified array, which is the shape github_169
// reduced to (`char *b[argc]; *b = a;`).
int main(void)
{
  int n = 3;
  int a[n];

  *a = 7;

  __VERIFIER_assert(a[0] == 7); // *a assigns the first element of a VLA
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_deref_array_vla
// EXPECT: ^VERIFICATION SUCCESSFUL$
