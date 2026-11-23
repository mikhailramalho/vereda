#include <stdlib.h>

/* Under --malloc-zero-is-null the constant-size fast path returned the NULL
 * symbol without assigning it, so p kept an unconstrained value and free(p)
 * was reported as an invalid free. Unlike github_5398 there is no assume
 * here: this is the plain shape, where nothing constrains p but the call. */
int main(void)
{
  void *p = malloc(0);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6593
// Needs: --malloc-zero-is-null
// RUN: --force-malloc-success --malloc-zero-is-null
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
