#include <assert.h>
#include <stdlib.h>

void unknown_method(int *a);

int main()
{
  int *p = (int *) malloc(4);

  unknown_method(p);
  free(p); // invalid pointer freed
}
// Imported from ESBMC regression/esbmc/unknown_method_call_dyn
// Needs: --unknown-method-args-check
// RUN: --force-malloc-success --unknown-method-args-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
